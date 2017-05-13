/*
 * CRC.c
 */

#include <inttypes.h>

#include "CRC.h"

#define CRC32_DEFAULT_STARTREMAINDER	0
#define CRC32_POLYNOMIAL				      0xEDB88320

static uint32_t table[256];
static uint8_t have_table = 0;

static void calculate_table()
{
	if (have_table)
		return;

	/* Calculate CRC table. */
	for (int i = 0; i < 256; i++) {
		uint32_t rem = i;  /* remainder from polynomial division */
		for (int j = 0; j < 8; j++) {
			if (rem & 1) {
				rem >>= 1;
				rem ^= CRC32_POLYNOMIAL;
			} else
				rem >>= 1;
		}
		table[i] = rem;
	}
	have_table = 1;
}

uint32_t crc32(const uint8_t *data, uint32_t length)
{
  if (!have_table)
    calculate_table();
  
	uint32_t crc = ~CRC32_DEFAULT_STARTREMAINDER;
	const uint8_t *q = data + length;
	for (const uint8_t *p = data; p < q; p++) {
		uint8_t octet = *p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
}
