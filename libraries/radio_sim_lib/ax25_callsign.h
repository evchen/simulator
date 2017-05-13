#ifndef _AX25_CALLSIGN_H
#define _AX25_CALLSIGN_H


#include <stdint.h>

#define DEFAULT_CALLSIGN_TRXVU "SA0SAT"
#define DEFAULT_CALLSIGN_GS "MIST  "
#define CALLSIGN_LENGTH 7 

void addCallsign(uint8_t *ax25_packet, uint32_t* ax25_packet_length, uint8_t* data, uint16_t data_length);
void addCallsign_withoutflag(uint8_t *ax25_packet, uint32_t* ax25_packet_length, uint8_t* data, uint16_t data_length); 

#endif
