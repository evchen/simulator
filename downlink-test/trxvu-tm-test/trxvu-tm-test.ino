extern "C" {
  #include "debug2.h"
  #include "I2Cslave.h"
  #include "SlaveDefinitions.h"
  #include "trxvu_sim.h"
  #include "ax25_callsign.h"
  #include "kiss.h"
  #include "fifo.h"
 // #include <String.h>
}

#define SEND_COUNT_TC 1
#define SEND_NEXT_TC_LENGTH 2
#define SEND_NEXT_TC 3

uint8_t response[255];      // store response to be sent back
uint32_t response_length;   // store response length to be sent back

char ToCallsign[7] = "MIST  ";   // start up destination callsign
char FromCallsign[7] = "SA0SAT";  // start up source callsign

void setup() {

  Serial.begin(9600);
  debugSetup();
  //next_command = 0;
  I2C_setup(receiveCallback, transmitBeginCallback, transmitCompletedCallback);

  Queue* q = (Queue*) malloc(sizeof(Queue));
  queue_init(q);
}

void loop() {

   printDebugs();
}


void setDefaultToCallsign(uint8_t* callsign, uint32_t length, uint8_t* response, uint32_t* response_length ){    
    
    *response_length = 7;
    
    for(int i = 0 ; i < 7 ; i++){
        ToCallsign[i] = callsign[i];
        response[i]  = callsign[i];
    }    
}

void sendTMdefCallsign(uint8_t* telemtry, uint32_t length, uint8_t* response, uint32_t* response_length){

  uint8_t* ax25_callsign_packet;
  uint32_t ax25_callsign_packet_length;
 
  // pack native callsign
  ax25_callsign_packet  = addCallsign_withoutflag( uint32_t* ax25_callsign_packet_length, uint8_t* telemetry, uint32_t length);

  int error = enqueue(q, ax25_callsign_packet, ax25_callsign_packet_length);

  if(error)
    response = 
  
}

void process_command(uint8_t* command, uint32_t command_length, uint8_t* response, uint32_t* response_length){

    switch(command[0]){
        case SET_DEFAULT_TO_CALLSIGN : 
            setDefaultToCallsign(command + 1, command_length-1, response, response_length);
            return;
        case SEND_TM_DEF_CALLSIGN :
            sendTMdefCallsign(command + 1, command_length-1 , response, response_length);
            return;
            
    }  
}


void receiveCallback (uint8_t *data, uint32_t dataLength) {

  process_command(data, dataLength, response, &response_length);
 
  writeDebug((char*)data);
  
}


// when OBC does a transmit request
// decied what data to send to OBC
// MSP packet. instert data and packet size. (no bigger than buff size)
void transmitBeginCallback () {
  int i ;

  for( i = 0 ; i < response_length ; i++){
    
    currentPacket.MSP_packet[i] = response[i];
  }
  currentPacket.packetSize = response_length;
  
}

// when tranmit is done to OBC
void transmitCompletedCallback() {

  }



  







