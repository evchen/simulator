/*
 * Endian.c
 * 
 * Author: John Wikman
 *
 * Description:
 * TBW
 */

#include <inttypes.h>

#include "Endian.h"

/*
 *  Takes a 32-bit number and puts it at the destination in
 *  a big endian byte order.
 */
void toBigEndian32(uint8_t *dest, uint32_t number)
{
  dest[0] = (uint8_t) (number >> 24) & 0xff;
  dest[1] = (uint8_t) (number >> 16) & 0xff;
  dest[2] = (uint8_t) (number >> 8)  & 0xff;
  dest[3] = (uint8_t) (number >> 0)  & 0xff;
}

void toBigEndian16(uint8_t *dest, uint16_t number)
{
  dest[0] = (uint8_t) (number >> 8) & 0xff;
  dest[1] = (uint8_t) (number >> 0) & 0xff;
}

/*
 * Retrieves a 32-bit big endian number located at the
 * entered src pointer.
 */
uint32_t fromBigEndian32(const uint8_t *src)
{
  uint32_t res = 0;
  for (uint8_t i = 0; i < 4; i++) {
    res = res << 8;
    res += *(src + i);
  }

  return res;
}

uint16_t fromBigEndian16(const uint8_t *src)
{
  uint16_t res = 0;
  for (uint8_t i = 0; i < 2; i++) {
    res = res << 8;
    res += *(src + i);
  }

  return res;
}

