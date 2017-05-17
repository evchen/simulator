#include <inttypes.h>
#include <stdlib.h>

extern "C"{
  #include "I2C.h"
  #include "I2C_configuration.h"
  #include "trxvu_sim_lib.h"
 
}

void printDebug(char* message, int number){
  Serial.print(message);
  Serial.print(" ");
  Serial.print(number,DEC);
  Serial.println();
  Serial.flush();
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  I2C_start(I2C_BIT_RATE, I2C_TIMEOUT_RATE);
  //InitLTBL();
  Serial.println("started!");
  Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5000);
  int error;

  uint8_t send = 'A';
  
  error = I2C_write(0x08, &send, 1 );
  /*
  if(error){
    Serial.print("error");
  }
  else{
    Serial.print("No errors");
  }

  uint8_t data[1];
  error = I2C_read(0x08, data, 1 );
  if(error){
    printDebug("Error read ", error);
  }
  else{
    printDebug("working, data: ", (int)data[0]);
  }
  */
  
  unsigned char feedback[7];

  unsigned char adrs = 8;
  unsigned char clsign[7] = {'A', 'L', 'L', ' ', ' ', ' ', '0'};
  
  error = SimTrxvu_tcSetDefToClSign( adrs, clsign,  feedback );

  if(error){
    printDebug("Setting to Callsign: ", error);
  }

  char* message = (char*)feedback;
  Serial.print(message);
  Serial.println();
  
}
