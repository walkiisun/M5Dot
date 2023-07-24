# M5Dot

![VisualDiagram](Extras/Diagram.png)  

## Summary
This project aimed to build a cost-effective, reliable, and modular product that can collect data, write it to a database and visualize it. This project was built upon the Sensoweb Labatory's current NewDot product at The University of Georgia. The NewDot Product uses a Raspberry Pi computer and a seismic sensor. This project set out to develop a complementary project by integrating a more cost-effective microcontroller and an inertial measurement unit (IMU) sensor.

## 📁 M5 Stick-C 
### Summary
The Sensor Device for the project was selected to be an M5 stick-C, an off the shelf microcontroller with an ESP32 chip and a 6 axis inertial measurement unit. On the device, a script is written to collect, synthesize, and publish the data to the server.

### **Files**
**ThreePointEAP.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server.  

**SixPointEAP.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server.

**kaDemo.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server. Kalman filtering is used to smooth the data.

**oneAxisDemo.ino** - This file displays only one axis of data (the Z axis). The sample rate of this program is much faster than the other programs. This program was used to try to detect animal heart rate at the UGA Vet hospital.

## 📁 Server  
### Summary
For this project we used the a server hosted on the Sensorweb Labatory at The University of Georgia. The server hosts a number of files and programs for our project. They are attached below
### Files and Programs
**Broker** - This folder infludes the instilation file for the broker and a README.md that includes all of the instructions for installing the broker. In our project we used the EMQX MQTT broker. Once installed and running, a dashboard for for broker can he located at ipAddress:18083.

**MQTT_Recieve.py** - This file subscribes to a topic on the MQTT Broker listens for messages. When a message is recieved, it calls the ServerFile.py file and passes the message to it.

**ServerFile.py** - This file is called from MQTT_Revieve.py. This file parses the message that was sent over MQTT and writes it to a database. 









