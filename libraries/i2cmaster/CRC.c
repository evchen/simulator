/*
 * CRC.c
 */

#include <inttypes.h>

#include "CRC.h"

#define CRC32_DEFAULT_STARTREMAINDER	0
#define CRC32_POLYNOMIAL				      0xEDB88320

static uint16_t table16[256];
static uint8_t have_table16 = 0;

static uint32_t table32[256];
static uint8_t have_table32 = 0;

/* Calculated according to the elveti reference */
static void calculate_table16()
{
  /* Calculate CRC16 table */
  for (uint16_t i = 0; i < 256; i++) {
    uint16_t rem = 0;
    if ((i & 1)   != 0) rem ^= 0x1021;
    if ((i & 2)   != 0) rem ^= 0x2042;
    if ((i & 4)   != 0) rem ^= 0x4084;
    if ((i & 8)   != 0) rem ^= 0x8108;
    if ((i & 16)  != 0) rem ^= 0x1231;
    if ((i & 32)  != 0) rem ^= 0x2462;
    if ((i & 64)  != 0) rem ^= 0x48C4;
    if ((i & 128) != 0) rem ^= 0x9188;
    table16[i] = rem;
  }
  have_table16 = 1;
}

static void calculate_table32()
{
  /* Calculate CRC32 table. */
  for (int i = 0; i < 256; i++) {
    uint32_t rem = i;  /* remainder from polynomial division */
    for (int j = 0; j < 8; j++) {
      if (rem & 1) {
        rem >>= 1;
        rem ^= CRC32_POLYNOMIAL;
      } else {
        rem >>= 1;
      }
    }
    table32[i] = rem;
  }
  have_table32 = 1;
}

/* Calculates CRC16 checksum according to elveti */
uint16_t crc16(const uint8_t *data, uint32_t length)
{
  if (have_table16 == 0)
    calculate_table16();

  uint16_t crc = 0xFFFF;

  for (uint32_t i = 0; i < length; i++)
    crc = (crc << 8) ^ table16[((crc >> 8) ^ data[i]) & 0xFF];

  return crc;
}

/* Calculates checksum according to ISO 3309 */
uint32_t crc32(const uint8_t *data, uint32_t length)
{
  if (have_table32 == 0)
    calculate_table32();
  
  uint32_t crc = ~CRC32_DEFAULT_STARTREMAINDER;
 
  for (uint32_t i = 0; i < length; i++)
    crc = (crc >> 8) ^ table32[data[i] ^ (crc & 0xff)];

  return ~crc;
}
