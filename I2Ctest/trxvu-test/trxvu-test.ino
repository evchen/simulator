extern "C"{
  #include "crc.h"
  #include "debug2.h"
  #include "I2Cslave.h"
  #include "SlaveDefinitions.h"
//  #include "trxvu_sim.h"
//  #include "fifo.h"
  #include "kiss.h"
  #include "ax25_callsign.h"
}

uint8_t * response;
uint8_t response_length;

void setup() {
  
  //Serial.begin(9600);
  //SerialUSB.begin(9600);

  I2C_setup(receiveCallback, transmitbeginCallback, transmitcompleteCallback);

}
  
void loop() {

}

void receiveCallback(uint8_t* data, uint32_t data_length){
  //process_command(data, data_length, response, &response_length);

  response = (uint8_t* )malloc(sizeof(1));
  response[0] = data[0];
  response_length = data_length;
  
}



void transmitbeginCallback(){
  
  int i;
  //for( i = 0 ; i < response_length; i ++ ){
    currentPacket.MSP_packet[1] = 'A';
  //}
  currentPacket.packetSize = 1;

  // reset command to be processed  
}

void transmitcompleteCallback() {
  free(response);

}
