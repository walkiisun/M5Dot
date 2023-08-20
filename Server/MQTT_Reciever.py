import paho.mqtt.client as mqtt
import threading
from ServerFile import completeCurl
from config import GetInfo
from Utils import parseString, getString, write_influx


# Create MQTT client
client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe(topic)

def on_message(client, userdata, msg):
    message = msg.payload.decode()
    inputParam = message.split(" ")
    if(inputParam[0] == "shake"): # Change if different table name
        write_influx(parseString(message))
    print(f"Received message: {message}")

def mqtt_loop():
    client.loop_forever()

def main():
    config()
    
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(broker, port, 60)

    mqtt_thread = threading.Thread(target=mqtt_loop)
    mqtt_thread.start()


# Get the values from the yaml file
def config():
    global topic
    global broker
    global port
    
    values = GetInfo() 
    
    broker = values[0]
    port = values[1]
    topic = values[2]
    

if __name__ == '__main__':
    config()
    main()
