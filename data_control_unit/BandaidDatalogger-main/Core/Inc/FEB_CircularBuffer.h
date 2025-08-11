#ifndef FEB_CIRCULARBUFFER_H
#define FEB_CIRCULARBUFFER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"

// Define CAN payload structure
struct FEB_CAN_Payload {
    uint32_t timestamp;
    uint32_t id;
    uint8_t data[8];
};

// Define circular buffer structure
typedef struct circBuffer {
    struct FEB_CAN_Payload buffer[64]; // Buffer array with 16 slots
    uint8_t IMU_DATA[23];  // TODO: Verify why IMU data length is 23
    size_t capacity;  // Maximum number of elements in buffer
    size_t count;     // Number of elements currently in buffer
    size_t write;     // Write index
    size_t read;      // Read index
} circBuffer;

// Function prototypes
void FEB_circBuf_sdcard_init(void);  // Initialize SD card once
void FEB_circBuf_init(circBuffer *cb);  // Initialize buffer structure
void FEB_circBuf_write(circBuffer *cb, uint32_t rec_id, uint8_t *rec_data);
void FEB_circBuf_read(circBuffer *cb);
void FEB_circBuf_dummy(circBuffer *cb);
void FEB_circBuf_addOrReplace(circBuffer *cb, uint32_t rec_id, uint8_t *rec_data);

#endif /* FEB_CIRCULARBUFFER_H */
