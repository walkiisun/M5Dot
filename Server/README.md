# M5Dot - Server

## Installation
1. **Download the Broker**: The broker for this project is EMQX.   
   
     A link to install the broker on the server can be found [here](https://docs.emqx.com/en/enterprise/v5.1/deploy/install.html).  The dashboard for the server can be found at: 

    ```
    <ipAddress>:18083
    ```
2. **Download  the "System Files" folder.** 
   
    To start listening to messages and writing them to a database, you will need to download the "System Files" folder. This folder contains the files that will listen for messages from the broker and write them to a database.

    Move into the System Folder on the server and run the following command:

    ```
    python3 MQTT_Revieve.py
    ```


## System Files
The M5Dot Project uses two files to listen for messages from the broker and write them to a database. The files are MQTT_Recieve.py and ServerFile.py. 

**MQTT_Recieve.py** - This file subscribes to a topic on the MQTT Broker listens for messages. When a message is recieved, it calls the ServerFile.py file and passes the message to it.

**ServerFile.py** - This file is called from MQTT_Revieve.py. This file parses the message that was sent over MQTT and writes it to a database.


## Troubleshooting
1. The packet size that the M5 Stick-C sends is 1024 bytes. If you are sending more than 1024 bytes, the message will be split into multiple packets. This will cause the server to crash. To fix this, you can either send less data or increase the packet size.
2. Double check the IP address of the broker in MQTT_Recieve.py. If the IP address is incorrect, the program will not work.
3. If you are having trouble with sensorweb.us you can also run the broker on a computer or a Raspberry Pi.
4. If you get a "-2" error on the serial monitor, that means that the packet size is too large. See tip 1.
