/*************************************************************
header for crc
*************************************************************/

#ifndef CRC_COM_H_
#define CRC_COM_H_

#ifdef __cplusplus
extern "C" {
	

	void InitLTBL();
}
#endif

uint16_t CRC_Optimized(uint8_t data, uint16_t check);


#endif // CCSDS_H
