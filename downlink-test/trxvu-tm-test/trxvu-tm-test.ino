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
Queue* q;

void setup() {
  SerialUSB.begin(9600);
  Serial.begin(9600);
  debugSetup();
  //next_command = 0;
  I2C_setup(receiveCallback, transmitBeginCallback, transmitCompletedCallback);

  q = (Queue*) malloc(sizeof(Queue));
  queue_init(q);
}

void loop() {
/*
  uint8_t* data;
  uint32_t data_length;

  uint8_t kiss_packet[511];
  uint32_t kiss_packet_length;
  
  if (dequeue(q, &data, &data_length)== 0){
    kiss_encode(kiss_packet, &kiss_packet_length, data, data_length);
    free(data);
    //SerialUSB.write(kiss_packet, kiss_packet_length);
  }
  */
 
  printDebugs();
}


void setDefaultToCallsign(uint8_t* callsign, uint32_t len, uint8_t* response, uint32_t* response_length ){    
    
    *response_length = 7;
    
    for(int i = 0 ; i < 7 ; i++){
        ToCallsign[i] = callsign[i];
        response[i]  = callsign[i];
    }    
}

void sendTMdefCallsign(uint8_t* telemetry, uint32_t len, uint8_t* response, uint32_t* response_length){

  uint8_t* ax25_callsign_packet;
  uint32_t ax25_callsign_packet_length;

  *response_length = 1;
  
  // pack native callsign
  ax25_callsign_packet  = addCallsign_withoutflag( & ax25_callsign_packet_length, telemetry, len);


  int avail = enqueue(q, ax25_callsign_packet, ax25_callsign_packet_length);

/*
  if(avail == -1)
    *response = DOWNLINK_BUFFER_FULL;
  else 
    *response = avail;*/
    
  
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
 /*  char debug[32]; debug[0] = 0;
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
  int i ;

  for( i = 0 ; i < response_length ; i++){
    
    currentPacket.MSP_packet[i] = response[i];
  }
  //currentPacket.MSP_packet[0] = response[0];
  currentPacket.packetSize = response_length;
  
  //currentPacket.packetSize = 1;
}

// when tranmit is done to OBC
void transmitCompletedCallback() {

  }



  






