# M5Dot

![VisualDiagram](Extras/Diagram.png)  

## Summary
This project aimed to build a cost-effective, reliable, and modular sensor that can collect data, write it to a database and visualize it. 

The sensor was built using an M5 Stick-C, an off the shelf microcontroller with an ESP32 chip and a 6 axis inertial measurement unit. The data was sent to a server via MQTT and written to a database. The data was then visualized using Grafana.

## ⚙️ Installation Instructions
The Installation for this product is as easy as three steps:
1. **Download the Broker**: The broker for this project is EMQX.   
   
     A link to install the broker on the server can be found [here](https://docs.emqx.com/en/enterprise/v5.1/deploy/install.html).  The dashboard for the server can be found at: 

    ```
    <ipAddress>:18083
    ```
2. **Download  the "System Files" folder.** 
   
    To start listening to messages and writing them to a database, you will need to download the "SystemFiles" folder. This folder contains the files that will listen for messages from the broker and write them to a database.

    Move into the System Folder on the server and run the following command:

    ```
    python3 MQTT_Reciever.py
    ```

3. **Pick which program you want to run and upload it to the M5 Stick-C.** For more information head to the [M5 Stick-C folder](https://github.com/walkiisun/M5Dot/tree/main/M5%20Stick-C). 

That is it!. Once the a program is downloaded onto the M5 Stick-C and the server files are downladed onto the server and started, the data will be collected and written to the database. The dashboards can be found [here](https://github.com/walkiisun/M5Dot/blob/main/Dashboards.md).


## Getting Started 
There are many files test files in the ServerFiles folder that can be used to "get started". To get started, follow the video and written instructions here.

## 📁 M5 Stick-C 
### Summary
The Sensor Device for the project was selected to be an M5 stick-C, an off the shelf microcontroller with an ESP32 chip and a 6 axis inertial measurement unit.   

On the device, a script is written to collect and publish the data to the server via MQTT.

### **Files**
Some scripts scripts collect acceleration data while others collect angular velocity data. Some scripts use Kalman filtering to smooth the data. Some scripts collect data at a high sample rate while others collect data at a low sample rate. 

For more information about the scripts, head to the [M5 Stick-C folder](https://github.com/walkiisun/M5Dot/tree/main/M5%20Stick-C)


## 📁 Server  
### Summary
For this project we used the a server hosted on the Sensorweb Labatory at The University of Georgia. The server hosts a number of files and programs for our project. They are attached below
### Files and Programs

📄 **MQTT_Recieve.py** - This file subscribes to a topic on the MQTT Broker listens for messages. When a message is recieved, it calls the ServerFile.py file and passes the message to it.

📄 **ServerFile.py** - This file is called from MQTT_Revieve.py. This file parses the message that was sent over MQTT and writes it to a database.

📄 **config.yaml** -  This files contains variables, usernames, ports, and passwords. 

📄 **config.py** -  This file contains contains a function to parse the config file. 

📄 **Test_Arduino_Send.py** -  This file creates an example sine wave and sends it to the server via MQTT. This file acts inplace of the arduino.

📄 **Test_MQTT_Recieve.py** -  This file subscribes to the topic in the config file and prints the message to the console.

📄 **Test_MQTT_Send.py** -  This file subscribes to the topic in the config file and publishes the message "Hello world".


## 📁 Test Files
This Folder contains files that were used to test the M5 Stick-C and the Server. It included test file for EAP connection, a file to write to influxDB, headless configuration file for the M5 Stick-C. These files are a work in progress.


## Troubleshooting 
1. Head to the broker  dashboard after setting up the broker on the server. If the broker is running, the dashboard should be running at ipAddress:18083. If the dashboard is not loading on the webpage, then the broker is not running.
2. If you are trying to look at Grafana, make sure you input lowercase letters of the mac address. For example, if the mac address is 30:AE:A4:1C:2B:1C, then the mac address inputed into Grafana should be 30:ae:a4:1c:2b:1c.







