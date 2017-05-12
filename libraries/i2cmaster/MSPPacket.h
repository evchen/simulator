/*
 * MSPPacket.h
 *
 * Author: John Wikman
 *
 * Description:
 * TBW
 */

#ifndef MSPPACKET_H
#define MSPPACKET_H

#include <inttypes.h>

typedef enum {
  SEND_HEADER,
  SEND_ACK,
  SEND_DATA,
  RECV_HEADER,
  RECV_DATA,
  RECV_ACK,
  DONE
} pack_state_t;

typedef struct msp_master_transmit {
  uint8_t addr;
  uint8_t msg_id;
  uint8_t seq_nr;
  uint8_t *data;
  uint16_t dl;
  uint16_t bytes_sent;
  pack_state_t state;
  int error;
} MSP_TX;


typedef struct msp_master_recieve {
  uint8_t addr;
  uint8_t msg_id;
  uint8_t seq_nr;
  uint16_t dl;
  uint8_t *buf;
  uint16_t buf_len;
  uint16_t bytes_recv;
  pack_state_t state;
  int error;
} MSP_RX;

/*
 * Sends an MSP packet to a slave.
 * 
 * This is a blocking function and will return once all the packets
 * have been sent or if a error is reached.
 */
int MSP_send(MSP_TX *pack);

/*
 * Recieves an MSP packet from a slave.
 */
int MSP_recv(MSP_RX *pack);

/*
 * Sends the next packet in the defined MSP data sequence.
 */
void MSP_sendNextPacket(MSP_TX *pack);

/*
 * Recieves the next packet in the MSP data sequence.
 */
void MSP_recieveNextPacket(MSP_RX *pack);

/*
 * Returns 0 if there is no more data to send or if an error has occurred. Else 1.
 */
int hasMoreToSend(MSP_TX *pack);

/*
 * Returns 0 if there is no more data to recieve or if an error has occurred. Else 1.
 */
int hasMoreToRecieve(MSP_RX *pack);

MSP_TX MSP_createTX(uint8_t addr, uint8_t msg_id, uint8_t seq_nr, uint8_t *data, uint16_t dl);
MSP_RX MSP_createRX(uint8_t addr, uint8_t *buf, uint16_t buf_len);

#endif
