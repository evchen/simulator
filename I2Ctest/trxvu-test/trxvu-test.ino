extern "C" {
  #include "debug2.h"
  #include "I2Cslave.h"
  #include "SlaveDefinitions.h"
 // #include <String.h>
}

#define SEND_COUNT_TC 1
#define SEND_NEXT_TC_LENGTH 2
#define SEND_NEXT_TC 3

int next_command;
uint8_t next_tc[32];

void setup() {

  Serial.begin(9600);
  debugSetup();
  next_command = 0;


  I2C_setup(receiveCallback, transmitBeginCallback, transmitCompletedCallback);
}



uint32_t x;
void loop() {

   printDebugs();
}

char ascii_command[32];
// Called when a message are received
void receiveCallback (uint8_t *data, uint32_t dataLength) {
  writeDebug("receiveCallback-------------------");
  if (dataLength == 1) {
    next_command = data[0];
    writeDebug("setting next command to: ");
    writeDebug(itoa(data[0], ascii_command, 10));
    return;
}
  /*
  // debugging
  char debug[32]; debug[0] = 0;
  strcat(debug, "OBC: ");
  for (int i = 0; i < dataLength; i++)
    itoa(data[i], debug + strlen(debug), 16);
  writeDebug(debug);
  */
 }

// when OBC does a transmit request
// decied what data to send to OBC
// MSP packet. instert data and packet size. (no bigger than buff size)
void transmitBeginCallback () {
  currentPacket.MSP_packet[0] = 1;
  currentPacket.packetSize = 1;
  
}

// when tranmit is done to OBC
void transmitCompletedCallback() {

  }



  







