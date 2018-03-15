import io
import picamera
import cv2
import numpy as np

import smbus
import time
import os

bus = smbus.SMBus(1)


# This is the address we setup in the Arduino Program
address = 0x04

def writeNumber(value):
    bus.write_byte(address, value)
    
    # bus.write_byte_data(address, 0, value)
    return -1

def readNumber():
    number = bus.read_byte(address)
    # number = bus.read_byte_data(address, 1)
    return number


def datasend(recognize):

    if(recognize==True):   
        #bytesToSend = ConvertStringToBytes("1")
        #bus.write_i2c_block_data(i2c_address, i2c_cmd, bytesToSend)
        cv2.destroyAllWindows()
        writeNumber(1);
        time.sleep(15);
    else:
        cv2.destroyAllWindows()
        writeNumber(0);
        
        
        
    
    return
