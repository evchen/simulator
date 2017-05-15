

extern "C" {
  #include "fifo.h"
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Queue* queue ;


void setup() {
  Serial.begin(9600);
  Serial.println("begin");
  
  queue = (Queue*) malloc(sizeof(Queue));
  queue_init(queue);
  
  uint8_t data0[4] = {1,2,3,4};
  uint8_t data1[5] = {5,6,7,8,9};
  uint8_t data2[6] = {10,11,12,13,14,15};

  enqueue(queue, data0, 4);
  enqueue(queue, data1, 5);
  enqueue(queue, data2, 6);
  
  uint8_t* d0;
  uint8_t* d1;
  uint8_t* d2;

  uint8_t d0_len;
  uint8_t d1_len;
  uint8_t d2_len;

  int error;

  error = dequeue(queue, &d0, &d0_len);

  error = dequeue(queue, &d1, &d1_len);

  error = dequeue(queue, &d2, &d2_len);
    error = dequeue(queue, &d2, &d2_len);


    Serial.print("Error");
  Serial.print(error);
  Serial.println();


  int i;

  for(i = 0; i < d0_len ; i ++){
    Serial.print("length is");
    Serial.println(d0_len);
    Serial.print(d0[i]);
    Serial.println();
  }

  
  for(i = 0; i < d1_len ; i ++){
    Serial.print("length is");
    Serial.println(d1_len);
    Serial.print(d1[i]);
    Serial.println();
  }
  
  
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(5000);
  
}
