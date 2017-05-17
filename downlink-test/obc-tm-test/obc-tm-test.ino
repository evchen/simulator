#include <inttypes.h>
#include <stdlib.h>

extern "C"{
  #include "I2C.h"
  #include "I2C_configuration.h"
  #include "crc.h"
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
  InitLTBL();
  Serial.println("started!");
  Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:

  int error;    // catching error message

  

  
}
