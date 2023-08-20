import paho.mqtt.client as mqtt
import time
from config import GetInfo
from Utils import getString

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
        message = getString()
        client.publish(topic, message)
        time.sleep(2)



if __name__ == '__main__':
    main()   
    