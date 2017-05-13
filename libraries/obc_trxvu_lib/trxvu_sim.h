#ifndef _TRXVU_SIM_H
#define _TRXVU_SIM_H


int SimTrxvu_tcSendAX25DefClSign ( unsigned char index, unsigned char *data, unsigned char length, unsigned char* avail);

int SimTrxvu_tcSendAX25OvrClSign ( unsigned char index, unsigned char * fromCallsign, unsigned char* toCallsign, unsigned char* data, unsigned char length, unsigned char* avail);

#endif
