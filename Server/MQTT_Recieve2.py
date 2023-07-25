import paho.mqtt.client as mqtt
import threading
from ServerFile import completeCurl

# MQTT broker information
broker = "172.21.127.197"
port = 1883
topic = "M5SleepDemo"

# Create MQTT client
client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe(topic)

def on_message(client, userdata, msg):
    message = msg.payload.decode()
    inputParam = message.split(" ")
    if(inputParam[0] == "M5"): # Change if different table name
        completeCurl(message)
    print(f"Received message: {message}")

def mqtt_loop():
    client.loop_forever()

def main():
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(broker, port, 60)

    mqtt_thread = threading.Thread(target=mqtt_loop)
    mqtt_thread.start()

    # Other code in the main thread can continue running here

if __name__ == '__main__':
    main()
