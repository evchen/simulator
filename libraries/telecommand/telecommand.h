/*************************************************************
header for decoding telecommand
*************************************************************/
#include <stdint.h>

#ifndef TELECOMMAND_H
#define TELECOMMAND_H






#define PACKET_APID(packet) (uint16_t) (packet[0]&7<<8 | packet[1])
#define PACKET_LENGTH(packet) (uint16_t) (packet[4]<<8 | packet[5])
#define PACKET_SERVICE_TYPE(packet) packet[7]

#endif // CCSDS_H