#ifndef _TRXVU_SIM_H
#define _TRXVU_SIM_H

// error definitions
#define NO_ERROR 0
#define I2C_INITIALIZATION_ERROR    2
#define I2C_COMMUNICATION_ERROR     4
#define DOWNLINK_FRAME_FULL         -2
#define INVALID_CALLSIGN            -4
#define PACKET_TOO_LARGE            -8

// I2C command definitions
#define SEND_TM_DEF_CALLSIGN        8
// #define SEND_TM_OVR_CALLSIGN        9
#define MAXIMUM_BUFFER_SIZE         235


/*! 
 * function is devied into 2 steps
 * 1. sending a byte to indicate the command for trxvu
 * 2. sending data to trxvu .
 * @param index I2C address
 * @param data data to be sent downinked
 * @param length length of data to be downlinked
 * @param avail pointer to buffer, where the available frame count will be stored
 * @ note    
 */

int SimTrxvu_tcSendAX25DefClSign ( unsigned char index, unsigned char *data, unsigned char length, unsigned char* avail);


#endif
