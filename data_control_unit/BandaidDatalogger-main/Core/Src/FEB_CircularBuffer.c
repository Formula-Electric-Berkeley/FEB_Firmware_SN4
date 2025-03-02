#include "fatfs.h"
#include "fatfs_sd.h"
#include "FEB_CircularBuffer.h"

extern UART_HandleTypeDef huart2;

FATFS fs; //file system
FIL fil; //file
FRESULT fres; //store result

UINT br,bw; //file read/write count

char buffer[1024]; // store data

//Capacity related stuff
FATFS *pfs;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;

//number of writes since last sync. Sync every 5 reads
uint8_t iteration;

void FEB_circBuf_init(circBuffer *cb)
{

  //Init cb
    if (cb->buffer == NULL) {
    printf("Error! Memory not allocated for buffer.");
    exit(0);
  }

  for (size_t i = 0; i < 16; i++) {
    memset(cb->buffer[i].data, 0, 8);
    cb->buffer[i].id = 0;
  }

  cb->capacity = 16;
  cb->count = 0;
  cb->write = 0;
  cb->read = 0; 


  // Mount SD card
//  fres = f_mount(&fs,"",0);
  fres = f_mount(&fs, "",1);
  if (fres!= FR_OK){
	  HAL_UART_Transmit(&huart2, "error mounting SD Card ...\n",27, HAL_MAX_DELAY);
  }else{
	  HAL_UART_Transmit(&huart2, "SD Card mounted successfully...\n", 32,HAL_MAX_DELAY);
  }

  f_getfree("", &fre_clust, &pfs);
  totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
  sprintf (buffer, "SD CARD Total Size: \t%lu\n", totalSpace);
  HAL_UART_Transmit(&huart2 , (uint8_t*)buffer,30, HAL_MAX_DELAY);

  freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);
  sprintf (buffer, "SD CARD Free Space: \t%lu\n",freeSpace);

  HAL_UART_Transmit(&huart2 , (uint8_t*)buffer,30, HAL_MAX_DELAY); 

  //TODO: Store file # in a header file. Open file, read number and increment. The incremented number is added to file name
  // thus on startup, we automatically log data into a new file. 


//  fres = f_open(&fil, "file1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
  fres = f_open(&fil, "test.csv", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
  if(fres != FR_OK){
    HAL_UART_Transmit(&huart2, "Can't open file\n",16,HAL_MAX_DELAY);
    return;
  }

  iteration = 0; 
}

// buffer to write data
char strToWrite[128];
char messToWrite[128];

// Check if maximum cappacity hasn't been reached. Copy string input, item, to heap and add pointer to buffer. Increment count of pointers and write index.
void FEB_circBuf_write(circBuffer *cb, uint32_t rec_id , uint8_t *rec_data)
{
  if (cb->count == cb->capacity) {
    printf("Error! No space to write.");
    return;
  }

  memcpy(cb->buffer[cb->write].data, rec_data, 8);
  cb->buffer[cb->write].id = rec_id;
  cb->buffer[cb->write].timestamp = HAL_GetTick();
//  cb->buffer[cb->write].timestamp = rec_timestamp; // uses global Timestamp (TransmitGlobalClock enabled)
  cb->write = (cb->write + 1) % cb->capacity;
  cb->count++;
}

// Check if buffer isn't full. Print earliest written string then free its space on heap. Increment read index and decrement count of pointers.
void FEB_circBuf_read(circBuffer *cb){

  char str[50]; 
  if (cb->count == 0) {
    printf("Error! Nothing to read.");
    return;
  }

  // print timestamp
  int size_len = sprintf(str, "Time(ms): %ld \n", cb->buffer[cb->read].timestamp);
  HAL_UART_Transmit(&huart2, (uint8_t*) str, size_len , HAL_MAX_DELAY);

  fres = f_lseek(&fil, f_size(&fil));
  if(fres != FR_OK){
    HAL_UART_Transmit(&huart2, "Can't find eof\n",15, HAL_MAX_DELAY);
    f_close(&fil);
    return;
  }

  // WRITE DATA TO .csv
  int len = snprintf(strToWrite, sizeof(strToWrite), "%u, %u", cb->buffer[cb->read].timestamp, cb->buffer[cb->read].id);
  for (int j = 0; j < 8; j++) {
	  len += snprintf(strToWrite + len, sizeof(strToWrite) - len, ", %u", cb->buffer[cb->read].data[j]);
  }
  strcat(strToWrite, "\n"); // Add a newline at the end
  fres = f_write(&fil, strToWrite, sizeof(strToWrite), &bw);


  // write Basic



  if(fres != FR_OK){
    HAL_UART_Transmit(&huart2, "Can't write timestamp\n",19, HAL_MAX_DELAY);
    f_close(&fil);
    return;
  }

  size_len = sprintf(str, "ID: %ld \n", cb->buffer[cb->read].id);
  HAL_UART_Transmit(&huart2, (uint8_t*) str, size_len , HAL_MAX_DELAY);

  fres = f_lseek(&fil, f_size(&fil));
  if(fres != FR_OK){
    HAL_UART_Transmit(&huart2, "Can't find eof\n",15, HAL_MAX_DELAY);
    f_close(&fil);
    return;
  }

//  fres = f_write(&fil, (const void *)&cb->buffer[cb->read].id, sizeof(uint32_t), &bw);
//  if(fres != FR_OK){
//    HAL_UART_Transmit(&huart2, "Can't write CAN id\n",19, HAL_MAX_DELAY);
//    f_close(&fil);
//    return;
//  }

  
  fres = f_lseek(&fil, f_size(&fil));
  if(fres != FR_OK){
    HAL_UART_Transmit(&huart2, "Can't find eof\n",15, HAL_MAX_DELAY);
    f_close(&fil);
    return;
  } 

//  fres = f_write(&fil,cb->buffer[cb->read].data, sizeof(cb->buffer[cb->read].data), &bw);
//  if(fres != FR_OK){
//    HAL_UART_Transmit(&huart2, "Can't write data\n",15, HAL_MAX_DELAY);
//    f_close(&fil);
//    return;
//  }


  fres = f_lseek(&fil, f_size(&fil));
  if(fres != FR_OK){
    HAL_UART_Transmit(&huart2, "Can't find eof\n",15, HAL_MAX_DELAY);
    f_close(&fil);
    return;
  }

//  fres = f_write(&fil,cb->IMU_DATA, sizeof(cb->IMU_DATA), &bw);
//  if(fres != FR_OK){
//    HAL_UART_Transmit(&huart2, "Can't write IMU data\n",15, HAL_MAX_DELAY);
//    f_close(&fil);
//    return;
//  }


  if(iteration >= 5){
    fres = f_sync(&fil);
    if(fres != FR_OK){
      HAL_UART_Transmit(&huart2, "Can't sync data\n",15, HAL_MAX_DELAY);
      f_close(&fil);
      return;
    } 
    iteration = 0; 
  }


  // print data
  for (int i = 0; i < 8; i++) {
    int size_len = sprintf(str, "\t Byte: %d \n", cb->buffer[cb->read].data[i]);
    HAL_UART_Transmit(&huart2, (uint8_t*) str, size_len , HAL_MAX_DELAY);
  }

  memset(cb->buffer[cb->read].data, 0, 8);
  cb->buffer[cb->read].id = 0;

  cb->read = (cb->read + 1) % cb->capacity;
  cb->count--;

  iteration++;
}

void FEB_circBuf_dummy(circBuffer *cb)
{
    uint8_t dummyData[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    // Random ID
    FEB_circBuf_write(cb, 0x123, dummyData);
}


