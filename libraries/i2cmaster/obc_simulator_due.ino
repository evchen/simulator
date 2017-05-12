/*
 * obc_simulator for the MIST project at KTH.
 * 
 * Written by John Wikman
 * 
 * Dependecies:
 * - Time.h (https://github.com/PaulStoffregen/Time)
 */

#include <inttypes.h>
#include <Time.h>

extern "C" {
  #include "I2C.h"
  #include "I2C_configuration.h"
  #include "Experiment.h"
  #include "MSPCommunication.h"
  #include "MSPErrorCodes.h"
}

#include "OBCSimulator.h"

#define ENABLE_DEBUG
#include "debug.h"

/* Generates MSP_COM with rx_buffer and tx_buffer as default */
#define MSP_generate(op,exp,tx_len) MSP_generateGeneric(op,exp,tx_buffer,tx_len,rx_buffer,sizeof(rx_buffer))

/* These must be global to please the stack */
static uint8_t tx_buffer[10000];
static uint8_t rx_buffer[10000];
static MSP_COM com;

void setup() {
  /* Initialize Serial communication with 9600 baud */
  Serial.begin(9600);
  
  debugln("##~~ SETUP START ~~##");

  /* Set time */
  setTime(0);

  /* Start I2C */
  I2C_start(I2C_BIT_RATE, I2C_TIMEOUT_RATE);

  /* Setup environment */
  setupSimulation(EXP_TEST, /* experiment id */
                  0x01, /* configID */
                  15, /* payload size */
                  0xEE, /* payload data */
                  200, /* bitflip chance */
                  3 /* bitflip region */);

  /* Setup polling frequencies */
  X_TEST.payload.freq = 5000; // 5Hz

  debugln("##~~ SETUP COMPLETE ~~##");
}

void loop() {
  // Main loop
  poll(EXP_TEST);
}

/** TEST **
 *  Tests requesting housekeeping from experiment.
 */
static void demoRequestHK() {
  debugln("# START OF TEST \"demoRequestHK\"");

  com = MSP_generate(MSP_OBC_REQ_HK, EXP_TEST, 0);

  /* Execute communication */
  MSP_executeAll(&com);

  summarizeTest();
}

/** TEST **
 * Tests requesting payload from experiment.
 */
static void demoRequestPayload() {
  debugln("# START OF TEST \"demoRequestPayload\"");
  
  com = MSP_generate(MSP_OBC_REQ_PAYLOAD, EXP_TEST, 0);

  /* Execute communication */
  MSP_executeAll(&com);

  summarizeTest();
}

/**
 * Summarizes the communications test that was just performed.
 * 
 * Prints the data received on success. Prints error message on
 * failiure.
 */
static void summarizeTest() {
  debugdecln("COM return code", com.code);

  if (!com.code) {
    debugln("Success!");
    /* Print a maximum of 15 bytes */
    printBytes(rx_buffer, com.r_pack.bytes_recv % 16);
  } else {
    /* Print error */
    char text[60];
    MSP_getErrorDescription(com.code, text);
    debug("## ERROR ##");
    debugln(text);
  }
  debugln("# END OF TEST");
}

/**
 * Prints an array of bytes in hexadecimal.
 */
static void printBytes(uint8_t *buf, size_t len) {
  if (len == 0)
    return;
    
  debugdec("Printing array of len", len);
  size_t i;
  for (i = 0; i < len-1; i++) {
    debughex("", buf[i]);
  }
  debughexln("", buf[i]);
}


/*
 * Simulates a poll for an experiment using the given frequencies.
 * 
 * This has to be improved to cope with error handling.
 */
static int poll(uint8_t exp_id)
{
  if (exp_id >= EXP_AMOUNT)
    return MSP_ERROR_INVALID_EXP_ID;

  /* Poll experiment for Payload */
  if (EXP_shouldPoll(&X_CONF(exp_id).payload, millis())) {
    com = MSP_generate(MSP_OBC_REQ_PAYLOAD, exp_id, 0);
    MSP_executeAll(&com);
  }

  /* Poll experiment for Housekeeping */
  if (EXP_shouldPoll(&X_CONF(exp_id).hk, millis())) {
    com = MSP_generate(MSP_OBC_REQ_HK, exp_id, 0);
    MSP_executeAll(&com);
  }

  /* Poll experiment for PUS Telemetry */
  if (EXP_shouldPoll(&X_CONF(exp_id).pus, millis())) {
    com = MSP_generate(MSP_OBC_REQ_PUS, exp_id, 0);
    MSP_executeAll(&com);
  }

  return 0;
}


