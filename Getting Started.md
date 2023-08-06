# Getting Started

There are many "test files" on the server that can be used to "get started". To get started download the "Server" Folder onto your server and follow the video and written instructions here.

It is a requirment to set up the server to complete the other tests below. After that, any test can be completed in any order.

## Set up the Broker
1. **Download the Broker**: The broker for this project is EMQX.   
   
     A link to install the broker on the server can be found [here](https://docs.emqx.com/en/enterprise/v5.1/deploy/install.html).  The dashboard for the server can be found at: 

    ```
    <ipAddress>:18083
    ```

## Testing the MQTT Communication
To test the broker and the MQTT communication, follow these steps:

1. Run the Test_MQTT_Recieve.py file on the server. 
   
```
python3 Test_MQTT_Recieve.py
```
   
2. Run the Test_MQTT_Send.py file on the server.

```
python3 Test_MQTT_Send.py
```

3. If the broker is running, the Test_MQTT_Recieve.py file should print "Hello World" to the console every three seconds.


## Testing the MQTT and Curl Communication
This demo is created to test the MQTT communication and the curl command. In the end, you should see a sine wave on the dashboard found [here](https://sensorweb.us:3000/d/M0m7iv6Vz/getting-started?orgId=1&from=now-5m&to=now&refresh=5s)

1. Run the Test_Arduino_Send.py file on the server. 
   
```
python3 Test_Arduino_Send.py
```
2. Run the MQTT_Reciever.py file on the server.

```
python3 MQTT_Reciever.py
```

1. This demo replaces with M5 Stick-C with a python script. The python script creates a sine wave and sends it to the server via MQTT. The MQTT_Reciever.py file recieves the message and writes it to the database.  
   
    If working properly, you should see a sine wave on the dashboard found [here](https://sensorweb.us:3000/d/M0m7iv6Vz/getting-started?orgId=1&from=now-5m&to=now&refresh=5s)

## Troubleshooting
1. Make sure the information in the config file is correct. All files on the server use the config file to get the information they need.