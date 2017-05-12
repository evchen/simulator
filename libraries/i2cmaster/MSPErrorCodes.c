/*
 * MSPErrorCodes.c
 *
 * Author: John Wikman
 *
 * Description:
 * Implements a function that produces a description
 * for each error.
 */

#include <stdio.h>
#include <string.h>

#include "MSPErrorCodes.h"

void MSP_getErrorDescription(int code, char *dest)
{
  switch (code) {
  case 0:
    strcpy(dest, "OK");
    break;
  case 4:
    strcpy(dest, "I2C: Write Error");
    break;
  case 5:
    strcpy(dest, "I2C: Read Error");
    break;
  case 6:
    strcpy(dest, "I2C: Timeout Error");
    break;
  case 7:
    strcpy(dest, "I2C: General Error");
    break;
  case MSP_ERROR_CREATE:
    strcpy(dest, "MSP: Communication creation error");
    break;
  case MSP_ERROR_INVALID_EXP_ID:
    strcpy(dest, "MSP: Entered invalid experiment ID");
    break;
  case MSP_ERROR_TX:
    strcpy(dest, "MSP: General transmit error");
    break;
  case MSP_ERROR_RX:
    strcpy(dest, "MSP: General receive error");
    break;
  case MSP_ERROR_ADDR:
    strcpy(dest, "MSP: ADDR field mismatch");
    break;
  case MSP_ERROR_MSGID:
    strcpy(dest, "MSP: Received unexpected Message ID");
    break;
  case MSP_ERROR_SEQNR:
    strcpy(dest, "MSP: Received invalid Sequence Number");
    break;
  case MSP_ERROR_CRC:
    strcpy(dest, "MSP: CRC mismatch");
    break;
  case MSP_ERROR_BUFFER:
    strcpy(dest, "MSP: Given buffer too small for incoming data");
    break;
  default:
    sprintf(dest, "Unknown: %d", code);
    break;
  }
}

