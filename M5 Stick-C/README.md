## 📁 M5 Stick-C 
### Summary
The Sensor Device for the project was selected to be an M5 stick-C, an off the shelf microcontroller with an ESP32 chip and a 6 axis inertial measurement unit.   

On the device, a script is written to collect, synthesize, and publish the data to the server.

### **Files**
📄 **ThreePointEAP.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server.  

📄 **SixPointEAP.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server.

📄 **kaDemo.ino** - This file is the script that runs on the M5 Stick-C. It collects data from the IMU and publishes it to the server. Kalman filtering is used to smooth the data.

📄 **oneAxisDemo.ino** - This file displays only one axis of data (the Z axis). The sample rate of this program is much faster than the other programs. This program was used to try to detect animal heart rate at the UGA Vet hospital.

📄 **CompleteSketch**.ino - This file displays only one axis of data (roll). This program is the only program to combine the combine the headless setup (DDC.ino) and the data collection scripts.