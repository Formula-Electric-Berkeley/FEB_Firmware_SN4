# BandaidDatalogger

# Description 
This branch is specifically for the data logging functionality of our DCU unit. If you're looking for any Driver UI 
related content, this is not the right branch. 

Our data logger consists of an STM32 connected to an SD card module, CAN Transceiver and eventually an Xbee. In order to 
store information onto the datalogger we use FATFS. 

# Devlog 

## 2/25/24 
As of today, we were able to mount the SD card and write strings. However, it may be more beneficial to write bytes instead 
of strings in the interest of performance.
	TODO: Get CAN loopback working and see if we can now combine CAN with the data logging functionality that has been completed 
	up to this point. 

 ## 3/3/24 
Moving over to saving data as byte arrays instead of strings. Split code into two sections which includes the CAN transmission and the data logging code. Get the CAN stuff working, then get the data logging stuff working. 
	TODO: Dorukhan should figure out the CAN stuff, Alex can probably figure out the data logging stuff.

## 4/15/24 
Alex finished converting the circular buffer to just bytes. At this point all we need to work on is the file system stuff with FAT FS. Some test functions were added to test 
Extended and Standard Ids 
	TODO: Finish filesystem related stuff. Must be able to write to a file.

## 4/16/24 
Data logging aspect has been finished. Should probably think about how to recover the sd card in the event any of the FATfs syscalls fail. Timestamps in ms has been added 
	TODO: Needs a new file system to create new file on startup, and to try restarting the file on syscall failure. Also needs python script.

## 5/26/24
Wrote code for IMU configuration and logging, need to test and debug. 