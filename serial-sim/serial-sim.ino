extern "C" {
  #include "ccsds.h"
  #include "crc.h"
  #include "ax25.h"
  #include "ax25_callsign.h"
  #include "kiss.h"
}

void setup() {
  // put your setup code here, to run once:
  InitLTBL();
  Serial.begin(9600);
  SerialUSB.begin(9600);
}

void printError(char* message, uint8_t error_code){
  Serial.print("Error occurred!!");
  Serial.print(error_code, DEC);
  Serial.print(F(message));
  Serial.print("\n");
  Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:

  uint8_t debug[4] = {0xC0, 0x00, 1, 0xC0};
  
  //SerialUSB.write( debug, 4);
  uint8_t data[4] = {1,2,3,4};
  uint8_t len = 4;
  uint8_t error;

  uint8_t ccsds_buffer[255];
  uint8_t ccsds_buffer_length;
  
  uint8_t ax25_buffer[255]; 
  uint8_t ax25_buffer_length;
  
  uint8_t ax25_callsign_buffer[255];  
  uint32_t ax25_callsign_buffer_length;
  
  uint8_t kiss_buffer[255];
  uint32_t kiss_buffer_length;

  error = CCSDS_GenerateTelemetryPacket(ccsds_buffer, &ccsds_buffer_length, 10, 1, 2, data, len, 0 );
  if (error) printError("packing ccsds",error);

  error = addAX25Frame(ax25_buffer, &ax25_buffer_length, ccsds_buffer, ccsds_buffer_length);
  if (error) printError("packing ax25", error);

  addCallsign_withoutflag(ax25_callsign_buffer, &ax25_callsign_buffer_length, ax25_buffer, ax25_buffer_length);

  kiss_encode(kiss_buffer, &kiss_buffer_length, ax25_callsign_buffer, ax25_callsign_buffer_length);

  
  SerialUSB.write(kiss_buffer, kiss_buffer_length);
  delay(5000);
}
