
extern "C" {
  #include "debug2.h"
  #include "I2Cslave.h"
  #include "SlaveDefinitions.h"
  #include "crc.h"
  #include "kiss.h"
  }

#define DEFAULT_CALLSIGN_TRXVU "SA0SAT"
#define DEFAULT_CALLSIGN_GS "MIST  "
#define CALLSIGN_LENGTH 7

void setup() {
  Serial.begin(9600);
  SerialUSB.begin(9600);
  debugSetup();

  I2C_setup(receiveCallback, transmitbeginCallback, transmitcompletedCallback);
}

uint32_t x;
uint32_t i;
boolean newData;
char buf[255];

void loop() {
  // put your main code here, to run repeatedly:
/*
  if(newReadSerialUSB()){
    writeDebug("Received data: ");
    writeDebug(buf);
  }
*/
  if (x++ % 1000000 == 0) {
    //writeDebug("alive");
    //SerialUSB.print("h");
  }
  
  printDebugs();
}

void addCallsign(uint8_t *ax25_packet, uint32_t* ax25_packet_length, uint8_t* data, uint16_t data_length) {
  
  uint32_t i;
  uint16_t chk;

  // calculating the length of ax25 packet
  *ax25_packet_length = data_length + 1 + 7 + 7 + 1 + 1 + 2 + 1;
  
  // add flag
  ax25_packet[0] = 0x7E;
  // add destincation callsign
  for ( i = 1 ; i < CALLSIGN_LENGTH; i ++ ) {
    ax25_packet[i] = (uint8_t) DEFAULT_CALLSIGN_GS[ i - 1 ] << 1;
  }

  // add ssid
  ax25_packet[CALLSIGN_LENGTH] = 0x60;
  
  // add source callsign
  for ( i = CALLSIGN_LENGTH+1 ; i <( CALLSIGN_LENGTH << 1) ; i ++ ){
    ax25_packet[i] = (uint8_t) DEFAULT_CALLSIGN_TRXVU[ i - 1 - CALLSIGN_LENGTH ] << 1;
  }

  ax25_packet[ CALLSIGN_LENGTH <<1 ] = 0x61;

  ax25_packet[15] = 0x3;   // control bits
  ax25_packet[16] = 0xF0;  // protocol identifier

  // adding data
  for ( i = 0 ; i < data_length ; i ++ ){
    ax25_packet[ i + 16 ] = data[i];
  }

  chk = 0xFFFF;
  // adding CRC
  for (i = 0; i < *ax25_packet_length - 3; i++) {
    chk = CRC_Optimized(ax25_packet[i], chk);
  }
  
  ax25_packet[*ax25_packet_length - 3] = (uint8_t)(chk >> 8);
  ax25_packet[*ax25_packet_length - 2] = (uint8_t)(chk);

  // add end flag
  ax25_packet[*ax25_packet_length - 1] = 0x7E;
  
  
}

// Called when a message are received
void receiveCallback (uint8_t *data, uint32_t dataLength) {
  /*
  //char debug[32]; debug[0] = 0;

  char debug[255];
  debug[0] = 0;
  strcat(debug, "OBC: ");
  
  // printout everything 
  
  int i;
  for( i = 0; i < dataLength ; i++){
  
    itoa(data[i], debug + strlen(debug), 16);
  
  }
  writeDebug(debug);
  */

  uint8_t ax25_data[255];
  uint32_t ax25_data_length;

  addCallsign(ax25_data, &ax25_data_length, data, dataLength);

  uint8_t kiss_data[520];
  uint32_t kiss_data_length;

  kiss_encode(kiss_data, &kiss_data_length, ax25_data, ax25_data_length);
  SerialUSB.write(kiss_data, kiss_data_length);
  
}

// when OBC does a transmit request
// decied what data to send to OBC
// MSP packet. instert data and packet size. (no bigger than buff size)
void transmitbeginCallback () {
  //writeDebug("OBC Transmit Request !!!!!");
  //example
  currentPacket.MSP_packet[0] = 7;
  currentPacket.packetSize = 1;
}

// when tranmit is done to OBC
void transmitcompletedCallback() {
  //writeDebug("Send complete");
  }







