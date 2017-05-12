/*
 * MSPSequence.h
 *
 * Author: John Wikman
 *
 * Description:
 * TBW
 */

#ifndef MSPSEQUENCE_H
#define MSPSEQUENCE_H

#include <inttypes.h>

typedef struct msp_sequencing {
  uint8_t current; /* contains the last received sequence number */
} MSP_SEQ;

/* Both the function and the define achieve the same result */
#define MSP_NEW_SEQUENCE {0}
MSP_SEQ MSP_newSequence();

/* Both src and dest should be an array of 2 bytes each */
void MSP_seqToBytes(MSP_SEQ *seq, uint8_t *dest);
void MSP_seqFromBytes(MSP_SEQ *seq, const uint8_t *src);

uint8_t MSP_nextSeqNr(MSP_SEQ *seq);
uint8_t MSP_currSeqNr(MSP_SEQ *seq);
void MSP_addSeqNr(MSP_SEQ *seq, uint8_t seq_nr);
void MSP_incSeqNr(MSP_SEQ *seq);

#endif
