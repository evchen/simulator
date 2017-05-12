/*
 * MSPErrorCodes.h
 *
 * Author: John Wikman
 *
 * Description:
 * Contains definitions of all error codes cast by the MSP protocol.
 * 
 * Errors in the span [-10,10] are I2C generated errors.
 */

#ifndef MSPERRORCODES_H
#define MSPERRORCODES_H

/* Communication errors */
#define MSP_ERROR_CREATE          11
#define MSP_ERROR_INVALID_EXP_ID  12

/* Frame errors */
#define MSP_ERROR_ADDR           -11
#define MSP_ERROR_MSGID          -12
#define MSP_ERROR_SEQNR          -13
#define MSP_ERROR_CRC            -14
#define MSP_ERROR_BUFFER         -15
#define MSP_ERROR_TX             -16
#define MSP_ERROR_RX             -17

/* destination needs to have at least 60 chars in buffer space */
void MSP_getErrorDescription(int code, char *dest);

#endif
