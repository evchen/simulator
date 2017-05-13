/*
 * Author: Johan Sjöblom
 */

#ifndef PUSLIST_H
#define PUSLIST_H

#define PUS_LIST_MAX_ITEMS 10
#define PUS_LIST_MAX_ITEM_SIZE 250

typedef struct {
  uint8_t data[PUS_LIST_MAX_ITEM_SIZE];
  uint32_t dataSize;
} PUS_Packet;

PUS_Packet *getPUSPacket();

void generatePUSAcknowledges(uint8_t *packet, uint32_t pSize);

#endif
