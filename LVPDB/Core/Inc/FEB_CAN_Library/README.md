# CAN Library

- [CAN Library](#can-library)
- [1 Overview](#1-overview)
- [2 Usage](#2-usage)
  - [2.1 Git Submodule](#21-git-submodule)
  - [2.2 Update Library](#22-update-library)
- [3 CAN Message Documentation](#3-can-message-documentation)
  - [3.1 BMS](#31-bms)
    - [3.1.1 State](#311-state)
    - [3.1.2 Cell Data](#312-cell-data)
    - [3.1.3 Accumulator Voltage](#313-accumulator-voltage)
    - [3.1.4 Accumulator Temperature](#314-accumulator-temperature)
  - [3.2 APPS](#32-apps)
    - [3.2.1 Normalized Brake](#321-normalized-brake)
    - [3.2.2 RMS Param Msg](#322-rms-param-msg)
    - [3.2.3 RMS Command Msg](#323-rms-command-msg)
    - [3.2.4 BSPD](#324-bspd)
    - [3.2.5 Current](#325-current)
  - [3.3 LVPDB](#33-lvpdb)
    - [3.3.1 Bus Current](#331-bus-current)
    - [3.3.2 Bus Voltage](#332-bus-voltage)
    - [3.3.3 CP Current](#333-cp-current)
    - [3.3.4 CP Voltage](#334-cp-voltage)
    - [3.3.5 AF Current](#335-af-current)
    - [3.3.6 AF Voltage](#336-af-voltage)
    - [3.3.7 RF Current](#337-rf-current)
    - [3.3.8 RF Voltage](#338-rf-voltage)
  - [3.4 DCU](#34-dcu)
  - [3.5 ICS](#35-ics)
    - [3.5.1 Buttons \& Swtiches](#351-buttons--swtiches)
  - [3.6 DART](#36-dart)
    - [3.6.1 Measured Fan Speeds (Both DART1 and DART2)](#361-measured-fan-speeds-both-dart1-and-dart2)

# 1 Overview

Branches:
* ```main```: Stores all files and latest changes.
* ```stm32```: Stores ```FEB_CAN_ID.h``` file. This branch is a shared submodule for all STM32 projects.

STM32 Files:
* ```FEB_CAN_ID.h```: A header file that stores all CAN Message IDs.
* ```FEB_CAN.c```: A file that handles CAN library initialization and received message handling. Additional CAN files will build of this one. File contains template code for writing additional CAN files.
* ```FEB_CAN.h```: Header file for ```FEB_CAN.c```.

Other files:
* ```FEB_CAN_ID.csv```: A CSV file that stores names of CAN messages without assigned CAN IDs. This file is used to generate dynamic CAN IDs.
* ```FEB_CAN_Static_ID.csv```: A CSV file that sores names of CAN messages and their assigned CAN ID.
* ```generate.py```: This file uses data from ```FEB_CAN_ID.csv``` and ```FEB_CAN_Static_ID.csv``` to generate ```FEB_CAN_ID.h```.
* ```feb_can_id.py```: A Python file that stores all CAN Message IDs.
* ```README.md```: Documentation for the CAN Library.

# 2 Usage

## 2.1 Git Submodule
1. <b>Add</b>: Install submodule into STM32 project, in Core/Inc/FEB_CAN_Library. Example command for BMS project is provided below.

   ```
   git submodule add -b stm32 git@github.com:Formula-Electric-Berkeley/FEB_CAN_Library_SN3.git BMS/Core/Inc/FEB_CAN_Library
   ```

   Command template.
   ```
   git submodule add -b stm32 <CAN Library GitHub URL> project_name/Core/Inc/FEB_CAN_Library
   ```
2. <b>Update</b>: Run this command periodically to update the CAN library for STM32 projects.
   ```
   git submodule update --init --remote --recursive
   ```

## 2.2 Update Library
Updating the library is a 2 step process, across branches ```main``` and ```stm32```.

<b>Git Branch</b>: ```main```

1. <b>Add CAN Message</b>: To add a CAN message with a static CAN ID, update ```FEB_CAN_Static_ID.csv``` file. To add a CAN message without a static CAN ID, update the ```FEB_CAN_ID.csv``` file.
2. <b>Generate</b>: Run ```generate.py``` script. This will update the ```FEB_CAN_ID.h``` header file.
3. <b>Documentation</b>: Document the new CAN message in this readme file.
4. <b>GitHub</b>: Commit and push changes to GitHub.

<b>Git Branch</b>: ```stm32```

1. <b>Update</b>: Fetch changes from main branch.
   
   ```
   git checkout main -- FEB_CAN_ID.h
   ```

2. <b>GitHub</b>: Commit and push changes to GitHub.

# 3 CAN Message Documentation
Note: Bytes and bits are 0-indexed. Multi-byte data is stored in Big-Endian format.

## 3.1 BMS
### 3.1.1 State
<table>
  <tr>
    <th>Byte</th>
    <th>Value</th>
    <th>Datatype</th>
  </tr>
  <tr>
    <td>0</td>
    <td>BMS State</td>
    <td>uint8_t</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Relay State</td>
    <td>-</td>
  </tr>
  <tr>
    <td>2</td>
    <td>GPIO Sense</td>
    <td>-</td>
  </tr>
</table>

<table>
  <tr>
    <th>BMS State Value</th>
    <th>BMS State</th>
  </tr>
  <tr>
    <td>0</td>
    <td>Startup</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Standby</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Balance</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Charge</td>
  </tr>
  <tr>
    <td>4</td>
    <td>Precharge</td>
  </tr>
  <tr>
    <td>5</td>
    <td>Drive-Standby</td>
  </tr>
  <tr>
    <td>6</td>
    <td>Drive</td>
  </tr>
  <tr>
    <td>7</td>
    <td>Drive-Regen</td>
  </tr>
  <tr>
    <td>8</td>
    <td>Fault</td>
  </tr>
</table>

<table>
  <tr>
    <th>Relay State Bit</th>
    <th>Value</th>
  </tr>
  <tr>
    <td>0</td>
    <td>BMS Shutdown</td>
  </tr>
  <tr>
    <td>1</td>
    <td>AIR+</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Precharge</td>
  </tr>
</table>

<table>
  <tr>
    <th>GPIO Sense Bit</th>
    <th>Value</th>
  </tr>
  <tr>
    <td>0</td>
    <td>AIR- Sense</td>
  </tr>
  <tr>
    <td>1</td>
    <td>AIR+ Sense</td>
  </tr>
  <tr>
    <td>2</td>
    <td>BMS Shutdown Sense</td>
  </tr>
  <tr>
    <td>3</td>
    <td>IMD Shutdown Sense</td>
  </tr>
  <tr>
    <td>4</td>
    <td>Charge Sense</td>
  </tr>
</table>

### 3.1.2 Cell Data
<table>
  <tr>
    <th>Byte</th>
    <th>Bit</th>
    <th>Value</th>
    <th>Unit</th>
    <th>Datatype</th>
  </tr>
  <tr>
    <td>0</td>
    <td>7-5</td>
    <td>Bank</td>
    <td>-</td>
    <td>uint8_t</td>
  </tr>
  <tr>
    <td>0</td>
    <td>4-0</td>
    <td>Cell</td>
    <td>-</td>
    <td>uint8_t</td>
  </tr>
  <tr>
    <td>1-2</td>
    <td>-</td>
    <td>Voltage</td>
    <td>mV</td>
    <td>uint16_t</td>
  </tr>
  <tr>
    <td>3-4</td>
    <td>-</td>
    <td>Temperature</td>
    <td>dC</td>
    <td>int16_t</td>
  </tr>
  <tr>
    <td>5</td>
    <td>-</td>
    <td>Flags</td>
    <td>-</td>
    <td>-</td>
  </tr>
</table>

<table>
  <tr>
    <th>Bit</th>
    <th>Flags</th>
  </tr>
  <tr>
    <td>0</td>
    <td>Under/Over Voltage</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Under/Over Temperature</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Balance</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Disabled Temperature</td>
  </tr>
</table>

### 3.1.3 Accumulator Voltage
<table>
  <tr>
    <th>Byte</th>
    <th>Value</th>
    <th>Unit</th>
    <th>Datatype</th>
  </tr>
  <tr>
    <td>0-1</td>
    <td>Total Pack Voltage</td>
    <td>mV</td>
    <td>uint16_t</td>
  </tr>
  <tr>
    <td>2-3</td>
    <td>Min Cell Voltage</td>
    <td>mV</td>
    <td>uint16_t</td>
  </tr>
  <tr>
    <td>4-5</td>
    <td>Max Cell Voltage</td>
    <td>mV</td>
    <td>uint16_t</td>
  </tr>
</table>

### 3.1.4 Accumulator Temperature
<table>
  <tr>
    <th>Byte</th>
    <th>Value</th>
    <th>Unit</th>
    <th>Datatype</th>
  </tr>
  <tr>
    <td>0-1</td>
    <td>Average Cell Temperature</td>
    <td>dC</td>
    <td>int16_t</td>
  </tr>
  <tr>
    <td>2-3</td>
    <td>Min Cell Temperature</td>
    <td>dC</td>
    <td>int16_t</td>
  </tr>
  <tr>
    <td>4-5</td>
    <td>Max Cell Temperature</td>
    <td>dC</td>
    <td>int16_t</td>
  </tr>
</table>

## 3.2 APPS
### 3.2.1 Normalized Brake
<table>
    <tr>
        <th>Byte</th>
        <th>Value</th>
        <th>Datatype</th>
    </tr>
    <tr>
        <td>0-4</td>
        <td>Normalized Brake</td>
        <td>float</td>
    </tr>
</table>

### 3.2.2 RMS Param Msg
<table>
    <tr>
        <th>Byte</th>
        <th>Value</th>
        <th>Datatype</th>
    </tr>
    <tr>
        <td>0</td>
        <td>addr</td>
        <td>uint8_t</td>
    </tr>
    <tr>
        <td>2</td>
        <td>r/w command</td>
        <td>boolean</td>
    </tr>
    <tr>
        <td>3</td>
        <td>NA</td>
        <td>-</td>
    </tr>
    <tr>
        <td>4-5</td>
        <td>data</td>
        <td>uint8_t</td>
    </tr>
    <tr>
        <td>6-7</td>
        <td>NA</td>
        <td>-</td>
    </tr>
</table>

<table>
    <tr>
        <th>R/W Command Value</th>
        <th>State</th>
    </tr>
    <tr>
        <td>0</td>
        <td>Read</td>
    </tr>
    <tr>
        <td>1</td>
        <td>Write</td>
    </tr>
</table>

<table>
    <tr>
        <th>Addr Value </th>
        <th>State</th>
    </tr>
    <tr>
        <td>20</td>
        <td>Fault Clear</td>
    </tr>
    <tr>
        <td>148</td>
        <td>CAN Active Messages Lo Wor</td>
    </tr>
</table>

### 3.2.3 RMS Command Msg
<table>
    <tr>
        <th>Byte</th>
        <th>Value</th>
        <th>Datatype</th>
    </tr>
    <tr>
        <td>0-1</td>
        <td>Torque Command</td>
        <td>Torque</td>
    </tr>
    <tr>
        <td>2-3</td>
        <td>Speed Command</td>
        <td>Angular Velocity</td>
    </tr>
    <tr>
        <td>4</td>
        <td>Direction</td>
        <td>boolean</td>
    </tr>
    <tr>
        <td>5.0</td>
        <td>Inverter Enable</td>
        <td>boolean</td>
    </tr>
    <tr>
        <td>5.1</td>
        <td>Inverter Discharge</td>
        <td>boolean</td>
    </tr>
    <tr>
        <td>5.2</td>
        <td>Speedmode Enabled</td>
        <td>boolean</td>
    </tr>
    <tr>
        <td>6-7</td>
        <td>Command Torque Limited</td>
        <td>Torque</td>
    </tr>
</table>

### 3.2.4 BSPD
<table>
    <tr>
        <th>Byte</th>
        <th>Value</th>
        <th>Datatype</th>
    </tr>
    <tr>
        <td>0</td>
        <td>BSPD State</td>
        <td>uint8_t</td>
    </tr>
</table>

<table>
    <tr>
        <th>BSPD State Value</th>
        <th>BSPD State</th>
    </tr>
    <tr>
        <td>0</td>
        <td>False / Not Triggered</td>
    </tr>
    <tr>
        <td>1</td>
        <td>True / Triggered</td>
    </tr>
</table>

### 3.2.5 Current
<table>
    <tr>
        <th>Byte</th>
        <th>Value</th>
        <th>Datatype</th>
    </tr>
    <tr>
        <td>0-4</td>
        <td>TPS Current</td>
        <td>float</td>
    </tr>
</table>

</body>

## 3.3 LVPDB
The LVPDB transmits 1 byte messages that correspond to the voltage and currents on the main bus, coolant pump, accumulator fans, or radiator fans. 
### 3.3.1 Bus Current 
<table>
    <tr>
        <th>Byte</th>
        <th>Value</th>
        <th>Datatype</th>
    </tr>
    <tr>
        <td>1-2</td>
        <td>Bus Current</td>
        <td>int16_t</td>
  Bus current should be measured to the second decimal place. Intended to be measured in mA
  
 
    </tr>
</table>


## 3.4 DCU

## 3.5 ICS
### 3.5.1 Buttons & Swtiches
The DASH_Breakout Board transmits a 1 byte message corresponding to status of the I/O on the I/O expander, the bits in the message are broken down in the table below:
<table>
  <tr>
    <th>Bit</th>
    <th>Value</th>
    <th>Datatype</th>
  </tr>
  <tr>
    <td>0</td>
    <td>Buzzer State</td>
    <td>bool</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Button 1 (Ready-To-Drive)</td>
    <td>bool</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Button 2</td>
    <td>bool</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Button 3</td>
    <td>bool</td>
  </tr>
  <tr>
    <td>4</td>
    <td>Button 4</td>
    <td>bool</td>
  </tr>
  <tr>
    <td>5</td>
    <td>Switch 1 (Coolant Pump)</td>
    <td>bool</td>
  </tr>
  <tr>
    <td>6</td>
    <td>Switch 2 (Radiator Fans)</td>
    <td>bool</td>
  </tr>
  <tr>
    <td>7</td>
    <td>Switch 3 (Accumulator Fans)</td>
    <td>bool</td>
  </tr>
</table>

## 3.6 DART
### 3.6.1 Measured Fan Speeds (Both DART1 and DART2)
<table>
  <tr>
    <th>Byte</th>
    <th>Value</th>
    <th>Datatype</th>
  </tr>
  <tr>
    <td>0</td>
    <td>Measured fan speed for fan 1, with 0 being fan off and 255 being fan at full speed</td>
    <td>uint8_t</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Equivalent for fan 2</td>
    <td>uint8_t</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Equivalent for fan 3</td>
    <td>uint8_t</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Equivalent for fan 4</td>
    <td>uint8_t</td>
  </tr>
  <tr>
    <td>4</td>
    <td>Equivalent for fan 5</td>
    <td>uint8_t</td>
  </tr>
</table>
