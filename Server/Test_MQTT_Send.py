import paho.mqtt.client as mqtt
import time
from config import GetInfo


##
## This code simply sends the message "Hello World" to the MQTT broker every 3 seconds
## To listen to this message, and test for connection, run the MQTT_Recieve.py file
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
    return "Hello World"


if __name__ == '__main__':
    main()