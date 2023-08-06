import paho.mqtt.client as mqtt
import time
from config import GetInfo
import numpy as np

##
## This code sends the message a sample sinewave to the MQTT broker every 3 seconds
## To listen to this message, and test for connection, run the MQTT_Reciever file and head to: https://sensorweb.us:3000/d/M0m7iv6Vz/getting-started?orgId=1&refresh=5s
##


client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe(topic)

def on_message(client, userdata, msg):
    message = msg.payload.decode()
    #print(f"Received message: {message}")

def main():
    global topic
    values = GetInfo() 
    broker = values[0]
    port = values[1]
    topic = values[2]
    
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(broker, port, 60)
    client.loop_start()

    while True:
        message = GetMessageNow()
        client.publish(topic, message)
        time.sleep(3)

def GetMessageNow():
    timeStart = round(time.time() - 3, 3)
    timeEnd = round(time.time(), 3)
    
    x = np.linspace(timeStart, timeEnd, 30)
    y = 4 * np.sin(2 * np.pi * x / 3)

    returnString = "M5 measurement1 "
    
    for i in range(len(y) -2):
        returnString += str(round(y[i],2)) + ","
        
    returnString += str(round(y[len(y)-1],2)) + " "
    
    returnString += str(timeStart) + " "
    returnString += str(timeEnd) + " "
    returnString += "00:00:00:00:00:00"
    
    return returnString


if __name__ == '__main__':
    main()   
    