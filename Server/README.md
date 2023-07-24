# M5Dot - Server

## Summary
We used the sensorweb.us server for this project. The following are the components of the server.

- Broker
- System Files

## Broker
The broker for this project is EMQX. The dashboard for the server can be found at:

```
 <ipAddress>:18083
```

 
A link to install the broker on the server can be found [here](https://docs.emqx.io/broker/latest/en/getting-started/install.html). 


## System Files
The M5Dot Project uses two files to listen for messages from the broker and write them to a database. The files are MQTT_Recieve.py and ServerFile.py. 

**MQTT_Recieve.py** - This file subscribes to a topic on the MQTT Broker listens for messages. When a message is recieved, it calls the ServerFile.py file and passes the message to it.

**ServerFile.py** - This file is called from MQTT_Revieve.py. This file parses the message that was sent over MQTT and writes it to a database.

## Installation
Once you have installed the broker, download  the "System Files" folder from this repository. 

To start listening to messages and writing them to influxDB all you have to do is:

```
python3 MQTT_Revieve.py
```

## Tips
1. The packet size that the M5 Stick-C sends is 1024 bytes. If you are sending more than 1024 bytes, the message will be split into multiple packets. This will cause the server to crash. To fix this, you can either send less data or increase the packet size.
2. Double check the IP address of the broker in MQTT_Recieve.py. If the IP address is incorrect, the program will not work.
3. If you are having trouble with sensorweb.us you can also run the broker on a computer or a Raspberry Pi.
4. If you get a "-2" error on the serial monitor, that means that the packet size is too large. See tip 1.
