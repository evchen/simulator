#include <stdlib.h>
#include "../i2cmaster/I2C.h"
#include "../i2cmaster/I2C_configuration.h"
#include "trxvu_sim.h"



int SimTrxvu_tcSendAX25DefClSign(unsigned char index, unsigned char * data, unsigned char length, unsigned char* avail){

    int error;
    
    // check length of the packet
    if (length > MAXIMUM_BUFFER_SIZE)
        return PACKET_TOO_LARGE;
    
    // data for I2C feedback. 
    uint8_t *feedback[1];
    
    // initialize I2C connection
    // can be initialized elsewhere    
    error = I2C_start(I2C_BIT_RATE , I2C_TIMEOUT_RATE);    
    if(error) 
        return I2C_INITIALIZATION_ERROR;
    
    // write command to prepare to send data
    error = I2C_write((uint32_t) index, SEND_TM_DEF_CALLSIGN, 1);
    if(error)
        return I2C_COMMUNICATION_ERROR;
    
    error = I2C_read((uint32_t) index, feedback, 1);
    if(error)
        return I2C_COMMUNICATION_ERROR;
    
    /*
     * if feed back is 0, tranfer is ready to be done
     * else error occurred. 
     * detailed error coding is in trxvu_sim.h
     */
    if(feedback[0])
        return feedback[0];

    // send actual data to the radio
    error = I2C_write((uint32_t) index, data, length);
    if(error)
        return I2C_COMMUNICATION_ERROR;
    
    // receive feedback as frame left available
    error = I2C_read((uint32_t) index, feedback, 1);
    if(error)
        return I2C_COMMUNICATION_ERROR;
    
    // store frame left available
    *avail = feeback[0];
 
    return NO_ERROR;
    
    
}

int SimTrxvu_tcSendAX25DefClSign(unsigned char index, unsigned char* fromCallsign, unsigned char* toCallsign, unsigned char * data, unsigned char length, unsigned char* avail){

    int error;
    
    // data for I2C feedback. 
    uint8_t *feedback[1];
    
    // check length of the packet
    if (length > MAXIMUM_BUFFER_SIZE)
    return PACKET_TOO_LARGE;
    
    // initialize I2C connection
    // can be initialized elsewhere    
    error = I2C_start(I2C_BIT_RATE , I2C_TIMEOUT_RATE);    
    if(error) 
        return I2C_INITIALIZATION_ERROR;
    
    // write command to prepare to send data
    error = I2C_write((uint32_t) index, SEND_TM_OVR_CALLSIGN, 1);
    if(error)
        return I2C_COMMUNICATION_ERROR;
    
    error = I2C_read((uint32_t) index, feedback, 1);
    if(error)
        return I2C_COMMUNICATION_ERROR;
    
    /*
     * if feed back is 0, tranfer is ready to be done
     * else error occurred. 
     * detailed error coding is in trxvu_sim.h
     */
    if(feedback[0])
        return feedback[0];

    // send actual data to the radio
    error = I2C_write((uint32_t) index, data, length);
    if(error)
        return I2C_COMMUNICATION_ERROR;
    
    // receive feedback as frame left available
    error = I2C_read((uint32_t) index, feedback, 1);
    if(error)
        return I2C_COMMUNICATION_ERROR;
    
    // store frame left available
    *avail = feeback[0];
 
    return NO_ERROR;
    
    
}
