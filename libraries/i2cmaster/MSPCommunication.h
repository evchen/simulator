/*
 * MSPCommunication.h
 *
 * Author: John Wikman
 *
 * Description:
 * Contains definitions and convenient functions for
 * sending complete communications with the MSP protocol.
 */

#ifndef MSPCOMMUNICATION_H
#define MSPCOMMUNICATION_H

#include <stdlib.h>

#include "MSPPacket.h"
#include "MSPSequence.h"

/* Standard commands sent by both master and slave */
#define MSP_NULL              0x00
#define MSP_DATA              0x01

/* OBC Requests (0 bytes data) */
#define MSP_OBC_REQ_PAYLOAD   0x10
#define MSP_OBC_REQ_HK        0x11
#define MSP_OBC_REQ_PUS       0x12

/* EXP Transmissions */
#define MSP_EXP_SEND_DATA     0x20

/* OBC Transmissions */
#define MSP_OBC_SEND_TIME     0x30
#define MSP_OBC_SEND_PUS      0x31

/* Custom commands */
#define MSP_OBC_SEND_CONFIG   0x90

/* Reserved addresses */
#define MSP_POLL              0xA0

/* OBC Control Commands (0 bytes data) */
#define MSP_OBC_ACTIVE        0xF0
#define MSP_OBC_SLEEP         0xF1
#define MSP_OBC_POWER_OFF     0xF2

/*
 * Below is OBC SIMULATOR definitions only
 */

typedef enum {
  COM_SEND,
  COM_RECV,
  COM_DONE
} msp_state_t;

typedef enum {
  COM_REQUEST,
  COM_WRITE,
  COM_READ,
  COM_NULL,
  COM_UNDEFINED
} msp_behavior_t;

typedef struct msp_communication_flow {
  msp_behavior_t state;
  msp_state_t behavior;
  int code;
  uint8_t address; /* of recipient */
  MSP_TX t_pack;
  MSP_RX r_pack;
  MSP_SEQ *tx_seq;
  MSP_SEQ *rx_seq;
} MSP_COM;

/*
 * Sets returnCode to 0xFFFF in MSP_COM if failed to generate
 * 
 * experiment_id must be one of the experiments found in EXP_configuration.h.
 */
MSP_COM MSP_generateGeneric(uint8_t msg_id, uint8_t exp_id, uint8_t *tx_buffer, uint16_t tx_bufferSize, uint8_t *rx_buffer, uint16_t rx_bufferSize);

int MSP_executeAll(MSP_COM *com);
int MSP_execute(MSP_COM *com);

int MSP_canExecute(MSP_COM *com);

#endif
