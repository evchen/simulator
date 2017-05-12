/*
 * MSPCommunication.c
 *
 * Author: John Wikman
 *
 * Description:
 * This implements functions that are beyond the standard MSP
 * packet structure and instead focuses on the data flow of
 * the MSP protocol. The functions here generates an MSP
 * communication that abstracts away if you are sending
 * something or if you are recieving something (besides when
 * you are creating the communication).
 */

#include "Experiment.h"
#include "MSPCommunication.h"
#include "MSPPacket.h"
#include "MSPErrorCodes.h"
#include "MSPSequence.h"

#define ENABLE_DEBUG
#include "debug.h"

/**
 * Returns the behavior of a message id.
 */
static msp_behavior_t getBehavior(uint8_t msg_id)
{
  /* Special cases */
  if (msg_id == MSP_NULL)
    return COM_NULL;
  if (msg_id == MSP_POLL)
    return COM_READ;

  /* Custom commands */
  if (msg_id == MSP_OBC_SEND_CONFIG)
    return COM_WRITE;

  /* Check category */
  uint8_t init = msg_id & 0xF0;
  if (init == 0x10)
    return COM_REQUEST;
  if (init == 0x30)
    return COM_WRITE;
  if (init == 0xF0)
    return COM_WRITE;

  /* No match, undefined behavior */
  return COM_UNDEFINED;
}

/*
 * Returns the initial state of a communication with a
 * certain behavior.
 */
static msp_state_t getInitialState(msp_behavior_t behavior)
{
  if (behavior == COM_WRITE)
    return COM_SEND;
  if (behavior == COM_REQUEST)
    return COM_SEND;
  if (behavior == COM_READ)
    return COM_RECV;

  /* Undefined behavior, dont send */
  return COM_DONE;
}

/*
 * Convinient function for sending NULL to an experiment
 * Returns the error code recieved
 */
static int sendNull(MSP_COM *com)
{
  MSP_TX t_pack = MSP_createTX(com->address, MSP_NULL, MSP_currSeqNr(com->rx_seq), NULL, 0); //send the last received seq_nr
  MSP_sendNextPacket(&t_pack);
  return t_pack.error;
}

static int readNull(MSP_COM *com)
{
  MSP_RX r_pack = MSP_createRX(com->address, NULL, 0);
  MSP_recieveNextPacket(&r_pack);
  if (r_pack.error) {
    debugdecln("NULL received error", r_pack.error);
    return -1;
  }
  return r_pack.seq_nr;
}

static void handleErrors(MSP_COM *com)
{
  switch (com->code) {
  case MSP_ERROR_BUFFER:
  case MSP_ERROR_ADDR:
  case MSP_ERROR_MSGID:
  case MSP_ERROR_CRC:
    sendNull(com);
    break;
  default:
    break;
  }
}

/*
 * Generates an MSP communication.
 */
MSP_COM MSP_generateGeneric(uint8_t msg_id, uint8_t exp_id, uint8_t *tx_buffer, uint16_t tx_bufferSize, uint8_t *rx_buffer, uint16_t rx_bufferSize)
{
  MSP_COM com;

  com.behavior = getBehavior(msg_id);
  com.state = getInitialState(com.behavior);
  com.code = 0;

  if (exp_id >= EXP_AMOUNT) {
    com.code = MSP_ERROR_INVALID_EXP_ID;
    return com;
  }

  com.address = X_CONF(exp_id).addr;
  com.tx_seq = &X_CONF(exp_id).tx_seq;
  com.rx_seq = &X_CONF(exp_id).rx_seq;

  switch (com.behavior) {
  case COM_REQUEST:
    com.t_pack = MSP_createTX(com.address, msg_id, MSP_nextSeqNr(com.rx_seq), NULL, 0);
  case COM_READ:
    if (rx_buffer == NULL || rx_bufferSize == 0)
      com.code = MSP_ERROR_CREATE;
    else
      com.r_pack = MSP_createRX(com.address, rx_buffer, rx_bufferSize);
    break;
    
  case COM_WRITE:
    com.t_pack = MSP_createTX(com.address, msg_id, MSP_currSeqNr(com.tx_seq), tx_buffer, tx_bufferSize);
    break;
    
  case COM_NULL:
    com.t_pack = MSP_createTX(com.address, msg_id, MSP_currSeqNr(com.rx_seq), NULL, 0); //send the last received seq_nr
    break;
    
  default:
    // undefined behavior
    com.code = MSP_ERROR_CREATE;
  }

  return com;
}

/*
 * Executes an entire MSP communication.
 */
int MSP_executeAll(MSP_COM *com)
{
  while (com->state != COM_DONE && com->code == 0) {
    MSP_execute(com);

    if (com->code)
      handleErrors(com);
  }

  if (com->code == 0 && com->behavior == COM_REQUEST) {
    /* Increment sequence number if all went well */
    MSP_addSeqNr(com->rx_seq, com->r_pack.seq_nr);
    /* let experiment know all is good! */
    sendNull(com);
  }

  /* TODO: handle incrementation of TX sequence number */
  if (com->code == 0 && com->behavior == COM_WRITE) {
    /* Read what sequence number that slave received, try five times to get the correct one */
    int response = -2;
    for (uint8_t i = 0; i < 5 && response < 0; i++)
      response = readNull(com);
      
    if (response == MSP_currSeqNr(com->tx_seq)) {
      MSP_incSeqNr(com->tx_seq);
    } else {
      sendNull(com);
      com->code = MSP_ERROR_SEQNR;
    }
  }

  return com->code;
}

/*
 * Executes one packet in the MSP communication. It can
 * either be a recieve or a transmit.
 */
int MSP_execute(MSP_COM *com)
{
  debug("MSP execute");
  if (com->state == COM_DONE || com->code)
    return com->code;

  if (com->state == COM_SEND) {
    debug("TX");
    /* If we are sending packets */
    MSP_sendNextPacket(&com->t_pack);
    com->code = com->t_pack.error;

    if (com->t_pack.dl == com->t_pack.bytes_sent) {
      // Check if we are done or if we have to read packets from slave
      if (com->behavior == COM_REQUEST) {
        debug("Switching");
        com->state = COM_RECV;
      } else {
        debug("Done");
        com->state = COM_DONE;
      }
    }
  } else if (com->state == COM_RECV) {
    debug("RX");
    /* If we are recieving packets */
    MSP_recieveNextPacket(&com->r_pack);
    com->code = com->r_pack.error;

    if (com->r_pack.dl == com->r_pack.bytes_recv)
      com->state = COM_DONE;

    // No check for behavior here since we won't ever recieve then write.
  }
  debugln("");

  return com->code;
}

int MSP_canExecute(MSP_COM *com)
{
  return (com->state == COM_DONE || com->code) ? 0 : 1;
}

