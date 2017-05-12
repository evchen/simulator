/*************************************************************
header for crc
*************************************************************/

#ifndef CRC_H
#define CRC_H

#ifdef __cplusplus
extern "C" {
	

	void InitLTBL();
}
#endif

uint16_t CRC_Optimized(uint8_t data, uint16_t check);


#endif // CCSDS_H
