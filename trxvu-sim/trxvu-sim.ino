
extern "C"{
  #include "crc.h"
  #include "debug2.h"
  #include "I2Cslave.h"
  #include "SlaveDefinitions.h"
  #include "trxvu_sim.h"
  #include "fifo.h"
  #include "kiss.h"
  #include "ax25_callsign.h"
}



int command =0;
uint8_t error;
Queue* q;
int frame_available;

void setup() {
  // put your setup code here, to run once:
  // initialize downlink com port
  InitLTBL();
  Serial.begin(9600);
  SerialUSB.begin(9600);
  // initialize I2C
  I2C_setup(receiveCallback, transmitbeginCallback, transmitcompleteCallback);
  // initialize queue
  q = (Queue*)malloc(sizeof(Queue));
  queue_init(q);
}
  uint8_t* data;
  uint32_t data_length;
  uint8_t kiss_buffer[511];
  uint32_t kiss_buffer_length;
  
void loop() {
  // put your main code here, to run repeatedly:
  
  int element_in_buffer;
  element_in_buffer =  dequeue(q, &data, &data_length);
//Serial.print(element_in_buffer,DEC);

  free(data);
  if(element_in_buffer){
    kiss_encode(kiss_buffer, &kiss_buffer_length, data, data_length);
    SerialUSB.write(kiss_buffer, kiss_buffer_length);
  }
  
  //delay(5000);
  
}

void receiveCallback(uint8_t* data, uint32_t data_length){
  
  // to identify a command, the data length needs to be 1, the current command is 0
  if (command == 0 && data_length == 1){
    processCommand(data[0]);
    return;
  }

  // check if packet is too large to fit in 235
  if (data_length > MAXIMUM_BUFFER_SIZE){
     error = PACKET_TOO_LARGE;
     return;
  }
  
  getAvailableFrameCount(q, &frame_available);
  
  // if there is still space left in the buffer
  if (frame_available < 1){
    error = DOWNLINK_BUFFER_FULL;
    return;
  }
  
  uint8_t* ax25_callsign_buffer= malloc(sizeof(255));  
  uint32_t ax25_callsign_buffer_length;
 
  // add the default call sign
  addCallsign_withoutflag(ax25_callsign_buffer, &ax25_callsign_buffer_length, data, data_length);
  
  // put the data in the queue
  enqueue(q, ax25_callsign_buffer, ax25_callsign_buffer_length);

  error = 0;  
}

void processCommand(uint8_t comm){

  switch(comm){
    case SEND_TM_DEF_CALLSIGN: 
      command = SEND_TM_DEF_CALLSIGN;
      break;
    default:
      error = INVALID_COMMAND;
  }
  
}


void transmitbeginCallback(){

  // compose the first byte to be errors
  currentPacket.MSP_packet[0] = error;
  // compose the second byte to be available frame count

  currentPacket.MSP_packet[1] = (uint8_t) frame_available;
  currentPacket.packetSize = 2;

  // reset command to be processed
  command = 0;

  
}

void transmitcompleteCallback() {

}
