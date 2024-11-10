

#ifndef FEB_CIRCULARBUFFER_H
#define FEB_CIRCULARBUFFER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"

//File system includes


//typedef struct circBuffer CircularBuffer;

struct FEB_CAN_Payload {
  uint32_t timestamp;
  uint32_t id;
  uint8_t data[8];
};

typedef struct circBuffer {
  struct FEB_CAN_Payload buffer[16];
  uint8_t IMU_DATA[23]; // TODO: why is this 23
  size_t capacity; // max capacity of buffer -- number of string pointers buffer will contain
  size_t count;    // number of pointers in buffer
  size_t write;    // index of write
  size_t read;     // index of read
} circBuffer;


void FEB_circBuf_init(circBuffer *cb);

void FEB_circBuf_write(circBuffer *cb, uint32_t rec_id, uint32_t rec_timestamp, uint8_t *rec_data);
void FEB_circBuf_read(circBuffer *cb);

#endif /* FEB_CIRCULARBUFFER_H */
