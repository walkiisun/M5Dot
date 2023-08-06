import paho.mqtt.client as mqtt
from config import GetInfo


# Create MQTT client
client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe(topic)

def on_message(client, userdata, msg):
    message = msg.payload.decode()
    print(f"Received message: {message}")

def main():
    global topic
    values = GetInfo() 
    broker = values[0]
    port = values[1]
    topic = values[2]
    
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(broker, port, 60)
    client.loop_forever()

if __name__ == '__main__':
    main()