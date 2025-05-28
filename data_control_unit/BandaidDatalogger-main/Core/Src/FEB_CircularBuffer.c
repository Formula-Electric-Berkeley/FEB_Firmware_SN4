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


//----CIRCBUF INIT----//
/* Will mount SD card and initialize the CSV file depending on space */
void FEB_circBuf_init(circBuffer *cb)
{
   // Circular buffer initialization
   if (cb->buffer == NULL) {
    printf("Error! Memory not allocated for buffer.");
    exit(0);
  }

  for (size_t i = 0; i < 64; i++) {
    memset(cb->buffer[i].data, 0, 8);
    cb->buffer[i].id = 0;
  }

  cb->capacity = 64;
  cb->count = 0;
  cb->write = 0;
  cb->read = 0;

  // Mount SD card
  fres = f_mount(&fs, "",1);
  if (fres!= FR_OK){
	  HAL_UART_Transmit(&huart2, "Error mounting SD Card \r\n", 27, HAL_MAX_DELAY);
  }else{
	  HAL_UART_Transmit(&huart2, "SD Card mounted successfully \r\n", 32, HAL_MAX_DELAY);
  }

  f_getfree("", &fre_clust, &pfs);
  totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
  sprintf (buffer, "SD CARD Total Size: \t%lu]r\n", totalSpace);

  freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);
  sprintf (buffer, "SD CARD Free Space: \t%lu\r\n",freeSpace);

  //TODO: Store file # in a header file. Open file, read number and increment. The incremented number is added to file name
  // thus on startup, we automatically log data into a new file.



  // Buffers
  char newFileName[32];
  FRESULT fres;
  DIR dir;
  FILINFO fno;
  int maxRunNum = -1;  // Start at -1 so even 'run.csv' counts as 0

  // Open the directory (root in this case)
  fres = f_opendir(&dir, "");
  if (fres == FR_OK) {
      while (1) {
          fres = f_readdir(&dir, &fno);  // Read next file
          if (fres != FR_OK || fno.fname[0] == 0) break;  // Break on error or end of dir

          // Check if file name starts with "run" and ends with ".csv"
          if (strstr(fno.fname, "run") == fno.fname && strstr(fno.fname, ".csv")) {
              int runNum = 0;

              // Match "run.csv" as 0 or "runX.csv" where X is number
              if (strcmp(fno.fname, "run.csv") == 0) {
                  runNum = 0;
              } else {
                  sscanf(fno.fname, "run%d.csv", &runNum);
              }

              if (runNum > maxRunNum) {
                  maxRunNum = runNum;
              }
          }
      }
      f_closedir(&dir);
  }

  // Generate new file name
  if (maxRunNum < 0) {
      strcpy(newFileName, "run.csv");  // No runs found, use default
  } else {
      sprintf(newFileName, "run%d.csv", maxRunNum + 1);
  }

  // Open the file for writing
  fres = f_open(&fil, newFileName, FA_CREATE_ALWAYS | FA_WRITE);
  if (fres != FR_OK) {
      HAL_UART_Transmit(&huart2, "Can't open new run file\r\n", 26, HAL_MAX_DELAY);
      return;
  }
}

// Buffer for data write
char strToWrite[128];
char messToWrite[128];


//----CIRCBUF WRITE----//
/* Will check for space and write data into buffer */
void FEB_circBuf_write(circBuffer *cb, uint32_t rec_id, uint8_t *rec_data)
{
  if (cb->count == cb->capacity) {
    printf("Error! No space to write.\r\n");
    return;
  }

  // Write data into buffer
  memcpy(cb->buffer[cb->write].data, rec_data, 8);
  cb->buffer[cb->write].id = rec_id;
  cb->buffer[cb->write].timestamp = HAL_GetTick();

  cb->write = (cb->write + 1) % cb->capacity;
  cb->count++;
}

void FEB_circBuf_addOrReplace(circBuffer *cb, uint32_t rec_id, uint8_t *rec_data)
{
    // Check for existing CAN ID
    if (cb->count > 0) {
        size_t index = cb->read;
        for (size_t i = 0; i < cb->count; i++) {
            if (cb->buffer[index].id == rec_id) {
                // Replace data and timestamp
                memcpy(cb->buffer[index].data, rec_data, 8);
                cb->buffer[index].timestamp = HAL_GetTick();
                return;
            }
            index = (index + 1) % cb->capacity;
        }
    }

    // If ID doesn't exist, check space to write new
    if (cb->count == cb->capacity) {
        printf("Error! No space to write.\r\n");
        return;
    }

    // Add new message
    memcpy(cb->buffer[cb->write].data, rec_data, 8);
    cb->buffer[cb->write].id = rec_id;
    cb->buffer[cb->write].timestamp = HAL_GetTick();

    cb->write = (cb->write + 1) % cb->capacity;
    cb->count++;
}

//----CIRCBUF READ----//
/* Reads from the buffer and writes data to the CSV and syncs data */
void FEB_circBuf_read(circBuffer *cb) {
    char str[50];
    if (cb->count == 0) {
        printf("Error! Nothing to read.");
        return;
    }

    // Timestamp
    int size_len = sprintf(str, "Time(ms): %ld \r\n", cb->buffer[cb->read].timestamp);
    //HAL_UART_Transmit(&huart2, (uint8_t*) str, size_len , HAL_MAX_DELAY);

    // Move to end of file
    fres = f_lseek(&fil, f_size(&fil));
    if(fres != FR_OK){
        HAL_UART_Transmit(&huart2, "Can't find eof\r\n", 15, HAL_MAX_DELAY);
        f_close(&fil);
        return;
    }

    // Write CSV data
    int len = snprintf(strToWrite, sizeof(strToWrite), "%u, 0x%04X", cb->buffer[cb->read].timestamp, cb->buffer[cb->read].id);
    for (int j = 0; j < 8; j++) {
        len += snprintf(strToWrite + len, sizeof(strToWrite) - len, ", %u", cb->buffer[cb->read].data[j]);
    }
    strcat(strToWrite, "\n");

    fres = f_write(&fil, strToWrite, strlen(strToWrite), &bw);
    if(fres != FR_OK){
        //HAL_UART_Transmit(&huart2, "Can't write timestamp\n", 19, HAL_MAX_DELAY);
        f_close(&fil);
        return;
    }

    //HAL_UART_Transmit(&huart2, (uint8_t*)"Wrote\r\n", 20, HAL_MAX_DELAY);

    // Print ID
    size_len = sprintf(str, "ID: %ld \n\r", cb->buffer[cb->read].id);
    //HAL_UART_Transmit(&huart2, (uint8_t*) str, size_len , HAL_MAX_DELAY);

    // Print Byte Data
    for (int i = 0; i < 8; i++) {
        int size_len = sprintf(str, "\t Byte: %d \n\r", cb->buffer[cb->read].data[i]);
        //HAL_UART_Transmit(&huart2, (uint8_t*) str, size_len , HAL_MAX_DELAY);
    }

    // Update buffer read index
    memset(cb->buffer[cb->read].data, 0, 8);
    cb->buffer[cb->read].id = 0;
    cb->read = (cb->read + 1) % cb->capacity;
    cb->count--;

    // Sync Data Every 5 Reads
    iteration++;
    if(iteration >= 5){
        fres = f_sync(&fil);
        if(fres != FR_OK){
            HAL_UART_Transmit(&huart2, "Can't sync data \n\r", 15, HAL_MAX_DELAY);
            f_close(&fil);
            return;
        }
        iteration = 0;
    }
}

//----DUMMY FUNCTION----//
void FEB_circBuf_dummy(circBuffer *cb)
{
    uint8_t dummyData[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    // Random ID
    FEB_circBuf_write(cb, 0x123, dummyData);
}


