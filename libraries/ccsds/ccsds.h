/*************************************************************
header for ccsds
*************************************************************/

#ifndef CCSDS_H
#define CCSDS_H


#define TM_NONDATASIZE 16
#define TM_HEADER_SIZE 14

/*
	macros for retrieving values from telecommands
*/
#define PACKET_APID(packet) (uint16_t) (packet[0]&7<<8 | packet[1])
#define PACKET_LENGTH(packet) (uint16_t) (packet[4]<<8 | packet[5])
#define PACKET_SEQUENCE_COUNT(packet) (uint16_t) (packet[2]&3F<<8 | packet[3])
#define PACKET_SERVICE_TYPE(packet) packet[7]
#define PACKET_SERVICE_SUBTYPE(packet) packet[8]
#define TC_ACK(packet) packet[6]&0xF
#define TC_DATA(packet) packet+10
#define TC_DATA_LENGTH(packet) (uint16_t) (packet[4]<<8 | packet[5])-6

/*
	for arduino testing purpose
*/
#ifdef __cplusplus
extern "C" {
	void CCSDS_GenerateTelemetryPacket(uint8_t *telemetryBuffer, uint8_t *telemetryBufferSize, uint16_t apid,
		uint8_t serviceType, uint8_t serviceSubtype, uint8_t *sourceData, uint8_t sourceDataLength,
		uint32_t time);
}

#else

void CCSDS_GenerateTelemetryPacket(uint8_t *telemetryBuffer, uint8_t *telemetryBufferSize, uint16_t apid,
	uint8_t serviceType, uint8_t serviceSubtype, uint8_t *sourceData, uint8_t sourceDataLength,
	uint32_t time);

#endif


#endif // CCSDS_H