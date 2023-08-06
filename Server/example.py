import paho.mqtt.client as mqtt
import time
from config import GetInfo
import numpy as np


def GetMessageNow():
    timeStart = round(time.time() - 1, 3)
    timeEnd = round(time.time(), 3)
    
    x = np.linspace(timeStart, timeEnd, 30)
    y = 10 * np.sin(2 * np.pi * x / 1)

    returnString = "M5 measurement1 "
    
    for i in range(len(y) -2):
        returnString += str(round(y[i],2)) + ","
        
    returnString += str(round(y[len(y)-1],2)) + " "
    
    returnString += str(timeStart) + " "
    returnString += str(timeEnd) + " "
    returnString += "00:00:00:00:00:00"
    
    return returnString


if __name__ == '__main__':
    print(GetMessageNow()) 
    