# Libraries
import cantools

# CAN messages
import bms_messages as bms_msg
import pcu_messages as pcu_msg
import dash_messages as dash_msg
import lvpdb_messages as lvpdb_msg
import dcu_message as dcu_msg
import sensor_nodes_messsages as sensor_msg

import dart_messages as dart_msg
import iv_meter_messages as iv_meter_msg
import ping_pong_messages as ping_pong_msg



# List of functions, used to generate CAN messages.
MESSAGE_GEN_LIST = [
    #0x0-0x3
    bms_msg.get_bms_state,
    bms_msg.get_bms_cell_data,
    bms_msg.get_accumulator_voltage, 
    bms_msg.get_accumulator_temperature,
    bms_msg.get_accumulator_faults,
    #Leaving 5 IDs for space

    #0x9-0xA
    pcu_msg.normalized_brake,
    pcu_msg.bspd,

    #Leaving 5 IDs for space
    
    #0x10
    dash_msg.get_dash_buttons,

    #Leaving 5 IDs for space
    
    #0x16-0x18
    lvpdb_msg.get_lvpdb_flag_bus_voltage_lv_current,
    lvpdb_msg.get_lvpdb_cp_af_rf_sh_current,
    lvpdb_msg.get_lvpdb_L_AS_AB_current,

    #Leaving 5 IDs for space

    #0x1E - 0x27
    #finalize sensor node here
    # 4 tire - E
    # 2 linear - F
    # 2 strain - 20
    # 1 coolant PRES
    # 1 ther
    # 1 coolant read
    # 1 imu
    # 1 gps 
    # 1 steerring
    # 2 wheel speed - 27

    #Leaving 5 IDs for space

    #0x2D - 0x2E
    dart_msg.get_measured_fan_speeds_1234,
    dart_msg.get_measured_fan_speeds_5,

    #Leaving 5 IDs for space

    #0X34 - 0x3A
    bms_msg.get_tps_voltage_current,
    pcu_msg.get_tps_voltage_current,
    dash_msg.get_tps_voltage_current,
    dcu_msg.get_tps_voltage_current,
    #front_sensor
    #rear_sensor
    
    #0xa5-0xa7
    #RMS messages from inverter
    
    #0xc0-0xc1
    pcu_msg.rms_command_msg, 
    pcu_msg.rms_param_msg,

    #0xd0-0xd5
    pcu_msg.get_pcu_heartbeat,
    dash_msg.get_dash_heartbeat, 
    lvpdb_msg.get_lvpdb_heartbeat,
    dcu_msg.get_dcu_heartbeat,
    sensor_msg.get_front_sensor_heartbeat,
    sensor_msg.get_rear_sensor_heartbeat,

    #0xe0-0xe0
    ping_pong_msg.get_ping_pong_counter1,
    ping_pong_msg.get_ping_pong_counter2,
    ping_pong_msg.get_ping_pong_counter3,
    ping_pong_msg.get_ping_pong_counter4
]

# List of priority assignments for each CAN message above
MESSAGE_GEN_ID = [
    #BMS
    0, 1, 2, 3, 4, 

    #PCU break and BSDPD
    9, 10,

    #DASH
    16, 

    #LVPDB TPS
    22, 23, 24, 

    #Sensor Node
    # 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,

    #DART
    45, 46,

    #TPS Chips (Excluding LVPDB)
    52, 53, 54, 55, #56, 57,

    #PCU RMS (Inverter) Parameter and Command
    192, 193,

    #Heartbeats
    208, 209, 210, 211, 212, 213,

    #PING PONG
    224, 225, 226, 227
]

def main():
    # Load CAN messages
    messages = []
    for i in range(len(MESSAGE_GEN_LIST)):
        msg = MESSAGE_GEN_LIST[i](MESSAGE_GEN_ID[i])
        messages.append(msg)
    
    # Generate DBC file
    db = cantools.db.Database(messages=messages)
    cantools.db.dump_file(db, "gen/FEB_CAN.dbc")

if __name__ == "__main__":
    main()
