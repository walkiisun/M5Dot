## 📁 M5 Stick-C 
### Summary
The Sensor Device for the project was selected to be an M5 stick-C, an off the shelf microcontroller with an ESP32 chip and a 6 axis inertial measurement unit.   

On the device, a script is written to collect, synthesize, and publish the data to the server.

## ⚙️ Installation Instructions
The Installation for this product is as easy as two steps:
1. Pick which arduino script you want to run on the M5 Stick-C. Each script has a different purpose. **If you are looking to measure posture, use KaDemo.ino.**
2. Using the [arduino IDE](https://www.arduino.cc/en/software), upload the script to the M5 Stick-C. Be sure to change the wifi settings, port, and broker. Troubleshouting tips can be found below.

### **Files**
📄 **ThreePointEAP.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server.  

📄 **SixPointEAP.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server.

📄 **kaDemo.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server. Kalman filtering is used to smooth the data.

📄 **oneAxisDemo.ino** - This file displays only one axis of data (the Z axis). The sample rate of this program is much faster than the other programs. This program was used to try to detect animal heart rate at the UGA Vet hospital.

📄 **CompleteSketch**.ino - This file displays only one axis of data (roll). This program is the only program to combine the combine the headless setup (DDC.ino) and the data collection scripts.

## Troubleshooting 
1. Change the Topic, Broker, and Port to match the server's settings.
```
const char* mqtt_broker = "172.21.127.197";
const char* topic = "M5Test";
const int mqtt_port = 1883;
```
2. The onboard battery is quite small. If the M5 Stick-C is not plugged in, it will only last an hours maximum.