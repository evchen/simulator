/*
 * MSPSequence.c
 *
 * Author: John Wikman
 *
 * Description:
 * Provides sequencing functionality for the MSP protocol.
 */

#include "MSPSequence.h"
#include "Endian.h"

#define ENABLE_DEBUG
#include "debug.h"

/* Creates a new MSP Sequence */
MSP_SEQ MSP_newSequence()
{
  MSP_SEQ seq;
  seq.current = 0;

  return seq;
}

/* dest must be an array of 2 bytes */
void MSP_seqToBytes(MSP_SEQ *seq, uint8_t *dest)
{
  toBigEndian16(dest, seq->current);
}

/* src must be an array of 2 bytes */
void MSP_seqFromBytes(MSP_SEQ *seq, const uint8_t *src)
{
  seq->current = fromBigEndian16(src);
}

/* returns the next sequence number */
uint8_t MSP_nextSeqNr(MSP_SEQ *seq)
{
  return seq->current + 1;
}

uint8_t MSP_currSeqNr(MSP_SEQ *seq)
{
  return seq->current;
}

/* Adds a newly received sequence number to the sequence */
void MSP_addSeqNr(MSP_SEQ *seq, uint8_t seq_nr)
{
  seq->current = seq_nr;
}

void MSP_incSeqNr(MSP_SEQ *seq)
{
  seq->current += 1;
}

