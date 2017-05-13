#include <stdlib.h>
#include "../i2cmaster/I2C.h"
#include "../i2cmaster/I2Cconfiguration.h"
#include "trxvu_sim.h"

#define I2CADDRESS 0x08

int SimTrxvu_tcSendAX25DefClSign(unsigned char index, unsigned char * data, unsigned char length, unsigned char* avail){
    
    error = I2C_start()
    
    // write command to prepare to send data
    I2C_write(I2CADDRESS, PREPARE_TO_SEND_DATA, 1);
    
    /*
     * be ready to read the feed back data
     * 0 ready
     * 
    
    
}

