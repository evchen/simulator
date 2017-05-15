#ifndef _FIFO_H
#define _FIFO_H

#include <stdint.h>

/*!
 * the fifo queue implementation stores the telemetry that is supposed to be downlinked. 
 * @data_length in each queue element is the length of telemetry data with ax25 info field.
 * @data is the telemetry data with ax25 info field
 * 
 */
#define MAX_FRAME_IN_TELEMETRY_BUFFER 40


typedef struct _queue_element{
    uint8_t  data_length;
    uint8_t* data;
    struct _queue_element* next_element;
}Element;

typedef struct _fifo_Q{
    struct _queue_element* head;
    struct _queue_element* tail;
    int size;
}Queue;


void queue_init(Queue* q);

/*!
 * Dequeue the first Element of the Queue. 
 * @param q Queue to dequeue
 * @param data a buffer to store the data pointer, which points to where the data is stored
 * @param data_length a buffer to store the length of the data
 * @note this function will not always succeed. might encournter that there is nothing to dequeue
 * @return Error code
 * 0 no error
 * -1 nothing to dequeue
 */
int dequeue(Queue* q, uint8_t** data, uint8_t* data_length );


/*!
 * Dequeue the first Element of the Queue. 
 * @param q Queue to dequeue
 * @param data data pointer for where the data is stored
 * @param data_length length of the data that needs to be stored
 * @note this function will not always succeed. might encournter that there is nothing to dequeue
 */
void enqueue(Queue* q, uint8_t* data, uint8_t data_length);

void getAvailableFrameCount(Queue* q, int* availableFrameCount);

void queue_delete(Queue* q);


#endif
