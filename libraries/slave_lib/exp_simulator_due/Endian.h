/*
 * Endian.h
 * 
 * Author: John Wikman
 *
 * Description:
 * TBW
 */

#ifndef ENDIAN_H
#define ENDIAN_H

#include <inttypes.h>

void toBigEndian32(uint8_t *dest, uint32_t number);
void toBigEndian16(uint8_t *dest, uint16_t number);
uint32_t fromBigEndian32(const uint8_t *src);
uint16_t fromBigEndian16(const uint8_t *src);

#endif

