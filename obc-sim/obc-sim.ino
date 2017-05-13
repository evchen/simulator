

#include <inttypes.h>
//#include <String.h>

extern "C"{
  #include "I2C.h"
  #include "I2C_configuration.h"
  #include "ccsds.h"
  #include "ax25.h"
  #include "crc.h"
}


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  I2C_start(I2C_BIT_RATE , I2C_TIMEOUT_RATE);
  InitLTBL();
  Serial.print("started!");
  Serial.println();
  Serial.flush();  
}

void loop() {
  // put your main code here, to run repeatedly:

  //uint8_t data[2] = {0x18, 13};

  //I2C_write(0x08, data, 2);
  
  sendHKdata();
  delay(5000);
}

void sendHKdata(){
  
  uint8_t data[7] = {1,2,3,4,5,6,7};
  uint8_t ccsds_packet[255];
  uint8_t ax25_packet[255];

  uint8_t data_length=7;
  uint8_t ccsds_packet_length;
  uint8_t ax25_packet_length;

  uint8_t error;
  
  //pack ccsdsds
  error =  CCSDS_GenerateTelemetryPacket(ccsds_packet, &ccsds_packet_length, 10, 1, 2, data, data_length, 0);
  
  if(error){
    
    Serial.print("Error occurred during packing ccsds: ");
    Serial.print(error);
    Serial.println();
    Serial.flush();
    
  }
  
  //generate AX25
  error = addAX25Frame (ax25_packet, &ax25_packet_length, ccsds_packet, ccsds_packet_length);
  if(error){
    
    Serial.print("Error occurred during packing ax25: ");
    Serial.print(error);
    Serial.println();
    Serial.flush();
    
  }
  

  I2C_write(0x08, ax25_packet, ax25_packet_length);
  Serial.println();
  Serial.flush();

  
  
}

