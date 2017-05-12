/*
 * Author: Johan Sjöblom
 */

#include <inttypes.h>
#include "PUSGenerator.h"
#include "debug2.h"
#include "stdlib.h"

static int headIndex;
static int numItems;
static PUS_Packet list[PUS_LIST_MAX_ITEMS];

PUS_Packet *getPUSPacket() {
  if (numItems <= 0) {
    return NULL;
  }
  PUS_Packet returnItem = list[headIndex];
  headIndex = (headIndex + 1) % PUS_LIST_MAX_ITEMS;
  numItems--;
  return &returnItem;
}

void generatePUSAcknowledges(uint8_t *pusPacket, uint32_t pSize) {
  if (numItems >= PUS_LIST_MAX_ITEMS){
    writeDebug("PUSLIST FULL, ERROR!!!");
  }
  numItems++;
}
