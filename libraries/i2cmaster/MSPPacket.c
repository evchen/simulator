/*
 * MSPPacket.c
 *
 * Author: John Wikman
 *
 * Description:
 * Contains functions for sending and recieving frames in
 * the MSP protocol.
 *
 * The two functions of intrest are MSP_sendNextPacket and
 * MSP_recieveNextPacket. They contain all the functionality
 * of the MSP protocol such as splitting the transaction into
 * multiple frames and formatting each frame.
 * 
 * HEADER:
 * ------------------------------------------------------------------------------
 * |  ADDR  | MSG ID | SEQ NR |       DL       |              CRC               |
 * ------------------------------------------------------------------------------
 * 0        1        2        3       4        5        6       7       8       9
 * 
 * DATA: (n-bytes)
 * ----------------------------..            ..-------------------------------------------
 * |  ADDR  | MSG ID |  DATA                      DATA  |              CRC               |
 * ----------------------------..            ..-------------------------------------------
 * 0        1        2        3               n-5      n-4      n-3     n-2     n-1      n
 * 
 * Data Types of Fields:
 * CRC and DL are unsigned integers encoded in big-endian byte order.
 * ADDR, MSG ID and SEQ NR are unsigned 8-bit integers.
 * DATA is a byte sequence between 1 and MAXCONT in length.
 */

#include "MSPPacket.h"
#include "MSPErrorCodes.h"
#include "I2C.h"
#include "I2C_configuration.h"
#include "CRC.h"
#include "Endian.h"

#define ENABLE_DEBUG
#include "debug.h"

#define MSP_MAXCONT (I2C_BUFFER_SIZE - 6)

static uint8_t local_buffer[I2C_BUFFER_SIZE];

/* Formats the default header into local_buffer (len = 9) */
static void formatDefaultHeader(MSP_TX *pack)
{
  local_buffer[0] = (pack->addr << 1) | 0x00; // Master Write: R/W-bit = 0
  local_buffer[1] = pack->msg_id;
  local_buffer[2] = pack->seq_nr;
  toBigEndian16(local_buffer + 3, pack->dl);
  toBigEndian32(local_buffer + 5, crc32(local_buffer, 5));
}

/* Formats a data packet into local_buffer (len + 6) */
static void formatData(uint8_t addr, uint8_t *data, uint16_t len)
{
  local_buffer[0] = (addr << 1) | 0x00; // Master Write: R/W-bit = 0
  local_buffer[1] = 0x01;
  for (uint16_t i = 0; i < len; i++)
    local_buffer[i+2] = data[i];
  
  toBigEndian32(local_buffer + len + 2, crc32(local_buffer, len + 2));
}

/* Extracts the default header from local_buffer into the packet struct */
static void extractDefaultHeader(MSP_RX *pack)
{
  /* Validate crc */
  uint32_t crc = fromBigEndian32(local_buffer + 5);
  if (crc != crc32(local_buffer, 5)) {
    pack->error = MSP_ERROR_CRC;
    return;
  }

  /* Validate sender */
  if (local_buffer[0] != ((pack->addr << 1) | 0x01)) { // Master Read: R/W-bit = 1
    pack->error = MSP_ERROR_ADDR;
    return;
  }

  pack->msg_id = local_buffer[1];
  pack->seq_nr = local_buffer[2];
  pack->dl = fromBigEndian16(local_buffer + 3);

  /* Make sure that buffer is enough */
  if (pack->dl > pack->buf_len) {
    debugdec("DL", pack->dl);
    debugdec("BUF SIZE", pack->buf_len);
    pack->error = MSP_ERROR_BUFFER;
    return;
  }
}

/* Extracts data from local_buffer into the packet struct */
static void extractData(MSP_RX *pack)
{
  /* Calculate how much data we will recieve in this packet */
  uint16_t data_amount = (pack->dl - pack->bytes_recv) % (MSP_MAXCONT + 1);

  /* Validate crc */
  uint32_t crc = fromBigEndian32(local_buffer + 2 + data_amount);
  if (crc != crc32(local_buffer, 2 + data_amount)) {
    pack->error = MSP_ERROR_CRC;
    return;
  }

  /* Validate sender */
  if (local_buffer[0] != ((pack->addr << 1) | 0x01)) { // Master Read: R/W-bit = 1
    pack->error = MSP_ERROR_ADDR;
    return;
  }

  /* Validate msg_id */
  if (local_buffer[1] != 0x01) {
    debughex("recieved following msg id when reading data", local_buffer[1]);
    pack->error = MSP_ERROR_MSGID;
    return;
  }

  /* Copy over data */
  uint8_t *dest = pack->buf + pack->bytes_recv;
  for (uint16_t i = 0; i < data_amount; i++)
    dest[i] = local_buffer[i+2];

  pack->bytes_recv += data_amount;

  return;
}

/*
 * Sends a complete MSP transmission / communication.
 *
 * Returns the collected error code.
 */
int MSP_send(MSP_TX *pack)
{
  while (hasMoreToSend(pack))
    MSP_sendNextPacket(pack);

  return pack->error;
}

/*
 * Recieving a complete MSP transmission / communication.
 *
 * Returns the collected error code.
 */
int MSP_recv(MSP_RX *pack)
{
  while (hasMoreToRecieve(pack))
    MSP_recieveNextPacket(pack);

  return pack->error;
}

/*
 * Sends the next packet in the defined MSP data sequence.
 *
 * Does not return anything. If an error has occurred, it will
 * set the error field in pack to that error. User has to manually
 * check for error and end of transmission or use the function
 * hasMoreToSend.
 */
void MSP_sendNextPacket(MSP_TX *pack)
{
  debughex("SEND", pack->addr);
  uint16_t send_amount;

  switch (pack->state) {
  case SEND_HEADER:
    debughex("Header", pack->msg_id);
    debughex("SEQ", pack->seq_nr);
    debugdec("DL", pack->dl);
    formatDefaultHeader(pack);
    pack->error = I2C_write(pack->addr, local_buffer, 9);

    if (pack->dl > 0)
      pack->state = SEND_DATA;
    else
      pack->state = DONE;
    break;

  case SEND_DATA:
    debug("Data");
    send_amount = (pack->dl - pack->bytes_sent) % (MSP_MAXCONT + 1);
    formatData(pack->addr, pack->data + pack->bytes_sent, send_amount);

    pack->error = I2C_write(pack->addr, local_buffer, send_amount + 6);
    pack->bytes_sent += send_amount;

    if (pack->bytes_sent >= pack->dl)
      pack->state = DONE;
    break;
    
  default:
    debug("Error");
    pack->state = MSP_ERROR_TX;
  }
  return;
}

/*
 * Recieves the next packet in the MSP data sequence.
 *
 * Does not return anything. If an error has occurred, it will
 * set the error field in pack to that error and return. User
 * has to manually check for error and end of transmission or
 * use the function hasMoreToRecieve.
 */
void MSP_recieveNextPacket(MSP_RX *pack)
{
  debughex("RECV", pack->addr);
  uint16_t recv_amount;

  switch (pack->state) {
  case RECV_HEADER: /* Receive header */
    pack->error = I2C_read(pack->addr, local_buffer, 9);
    if (pack->error) {
      debug("Header");
      pack->state = DONE;
      return;
    }

    extractDefaultHeader(pack);
    debughex("Header", pack->msg_id);
    debughex("SEQ", pack->seq_nr);
    debugdec("DL", pack->dl);
    if (pack->error) {
      pack->state = DONE;
      return;
    }

    if (pack->dl > 0)
      pack->state = RECV_DATA;
    else
      pack->state = DONE;
    break;
    
  case RECV_DATA: /* Receive data */
    debug("Data");
    recv_amount = (pack->dl - pack->bytes_recv) % (MSP_MAXCONT + 1);
    pack->error = I2C_read(pack->addr, local_buffer, recv_amount + 6);
    if (pack->error) {
      pack->state = DONE;
      return;
    }

    extractData(pack);
    if (pack->error) {
      pack->state = DONE;
      return;
    }

    if (pack->bytes_recv >= pack->dl)
      pack->state = DONE;
    break;
    
  default:
    debug("Error");
    pack->state = MSP_ERROR_RX;
  }
  return;
}

/*
 * Returns 0 if there is no more data to send or if an error has occurred. Else 1.
 */
int hasMoreToSend(MSP_TX *pack)
{
  return (pack->error || (pack->bytes_sent >= pack->dl && pack->state != DONE)) ? 0 : 1;
}

/*
 * Returns 0 if there is no more data to recieve or if an error has occurred. Else 1.
 */
int hasMoreToRecieve(MSP_RX *pack)
{
  return (pack->error || (pack->bytes_recv >= pack->buf_len && pack->state != DONE)) ? 0 : 1;
}

/*
 * Initializes an MSP transmit communication.
 */
MSP_TX MSP_createTX(uint8_t addr, uint8_t msg_id, uint8_t seq_nr, uint8_t *data, uint16_t dl)
{
  MSP_TX pack;
  
  pack.addr = addr;
  pack.msg_id = msg_id;
  pack.seq_nr = seq_nr;
  pack.data = data;
  pack.dl = dl;
  pack.bytes_sent = 0;
  pack.state = SEND_HEADER;
  pack.error = 0;

  return pack;
}

/*
 * Initializes an MSP receive communication.
 */
MSP_RX MSP_createRX(uint8_t addr, uint8_t *buf, uint16_t buf_len)
{
  MSP_RX pack;

  pack.addr = addr;
  pack.msg_id = 0; // this will be set by MSP_recv
  pack.seq_nr = 0; // this will be set by MSP_recv
  pack.dl = 0;
  pack.buf = buf;
  pack.buf_len = buf_len;
  pack.bytes_recv = 0;
  pack.state = RECV_HEADER;
  pack.error = 0;

  return pack;
}

