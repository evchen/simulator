#include "fifo.h"
#include <stdlib.h>
#include <stdint.h>

void queue_init( Queue* q){

    q->size = 0;
    
    // allocate the head of the Queue
    
    // use for both head and tail of the Queue
    
    // set total Element in the Queue to be 0
}



 int dequeue(Queue* q, uint8_t** data, uint32_t* data_length){

     if((q-> size) < 1) return -1;
    
     // take the head of the Queue
     Element* head = q->head;
     Element* newhead = head->next_element;
     
     
     // reduce total Element in the Queue
     (q-> size) --;
     
     // copy data from the Queue to the return buffer
     *data_length = head ->data_length;
     *data = head->data;
     
     // free Element
     free(head);
     
     // return the head of Queue as the next of head
     q-> head = newhead;
     
     return 0;
     
}
 
 void enqueue(Queue* q, uint8_t* data, uint32_t data_length){
     
     Element* new = malloc(sizeof(Element));
     new -> data_length = data_length;
     new -> data = data;
     
     if ( q -> size )
     {
        q-> tail -> next_element = new;
     }
     else {
         q ->head = new;
     }

     
     q-> tail = new;
     
     q-> size ++;

     
 }
 
 void getAvailableFrameCount(Queue* q, int* availableFrameCount){
    
     *availableFrameCount = MAX_FRAME_IN_TELEMETRY_BUFFER - (q->size);
     // return the total frame count
 }
 
 void queue_delete(Queue* q){
     
     int i = 0;
     uint8_t* data;
     uint8_t data_length;
     while((q->size) && i ){
         i = dequeue(q, &data, &data_length);
         
     }
     
     free(q);
 }
