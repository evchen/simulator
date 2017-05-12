/************************************************************************
Decoding Telecommand
*************************************************************************/


#include <stdint.h>
#include <stdio.h>
#include "telecommand.h"
#include "crc.h"



/*
* get telecommand from arduino
*
*/
/*
void retrieve_telecommand_from_arduino(uint8_t (*retrieve)(void), uint8_t *data, uint32_t size) {
	int i;
	for (i = 0; i < size; i++) {
		data[i] = (*retrieve)();
	}
}


void retrieve_telecommand(char (*retrieve)(void), uint8_t* telecommand) {

	
	retrieve_telecommand_from_arduino((uint8_t)(*retrieve)(), telecommand, 6);

	uint16_t length = telecommand[4] << 8 | telecommand[5];

	retrieve_telecommand_from_arduino((uint8_t)(*retrieve)(), telecommand+6, length);

}
*/




