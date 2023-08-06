#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "time.h"
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks


const char* ssid = "sensorweb";
const char* password = "sensorweb128";

char* EAP_IDENTITY  = "SCHOOL EMAIL";
char* EAP_PASSWORD  = "SCHOOL PASSWORD";

char* STAusername;
char* STApassword;

//imu vars
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

float pitch = 0.0F;
float roll = 0.0F;
float yaw = 0.0F;

int direction = 0;  //0 = supine , 1 = right, 2 = prone, 3 = left

//String vars
String macAddress = WiFi.macAddress();  //"00:00:00:00:00:00";


TaskHandle_t Task1;

//time vars
double Epoch_Time;
double Epoch_millis;
double startMil;
double startTime;
double outTime;
float mqttMil;

String officalStart;
String officalEnd;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -18000;
const int daylightOffset_sec = 0;

const int numDataPoints = 100;

struct RowData {
  float accXArray[numDataPoints];
  float accYArray[numDataPoints];
  float accZArray[numDataPoints];
};

RowData rowData;

String unitArray[] = {
  "accX",
  "accY",
  "accZ",
};

int numOfUnits;
int count;
bool finishCollection = false;
int samplePeriord = 5; // 5 ms

//epoch vars

String numString;
String packetStartEpoch;
String packetStopEpoch;


WiFiClient espClient;
PubSubClient client(espClient);

//mqtt vars 
const char* mqtt_broker = "172.21.127.197";
const char* topic = "M5Hospital";
const int mqtt_port = 1883;

void Task1code(void* pvParameters) {
  for (;;) {
    getDataIMU();
    collectData();
  }
}

unsigned long Get_Epoch_Time() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

String epochString() {
  double syncMil = millis() - startMil;
  syncMil = syncMil / 1000;
  outTime = startTime + syncMil;
  numString = String(outTime, 3 );
  //Serial.println(numString);
  return numString;
}

void getDataIMU() {
  M5.IMU.getAccelData(&accX, &accY, &accZ);
}

String createOutMQTT(String startTime, String endTime, float* dataArray, String macAddress, String unit) {
  String result = "M5";
  result += " ";
  result += unit;
  result += " ";
  for (int i = 0; i < numDataPoints; i++) {
    result += String(dataArray[i]);
    if (i < numDataPoints - 1) {
      result += ",";
    }
  }
  result += " ";
  result += startTime;
  result += " ";
  result += endTime;
  result += " ";
  result += macAddress;  
  return result;
}

void bootMQTT() {
  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {

    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");

      Serial.println(client.state());
      Serial.println(client_id);
      delay(1000);
    }
  }
  client.publish(topic, "Hi EMQX I'm ESP32 ^^");
  client.setBufferSize(1023);
}


void collectData(){
  if(count < numDataPoints){
    if(count == 0){packetStartEpoch = epochString();} 

    rowData.accZArray[count] = accZ;

    count++;
  } 
  else{ // The last index of the array
    packetStopEpoch = epochString();

    officalEnd = packetStopEpoch;
    officalStart = packetStartEpoch;

    count = 0;
    finishCollection = true;
  }
  vTaskDelay(samplePeriord);
}

bool connectToEduroam(String InputUsername, String InputPassword) {
  bool returnStatus;
  char* ssid = "eduroam"; // eduroam SSID


  STAusername = &InputUsername[0];
  STApassword = &InputPassword[0];

  delay(10);
  Serial.println();

  WiFi.mode(WIFI_STA); //init wifi mode
  WiFi.begin(ssid, WPA2_AUTH_PEAP, STAusername, STAusername, STApassword); //without CERTIFICATE
  Serial.print(("Connecting to network: "));
  Serial.println(ssid);
  
  
  while ((WiFi.status() != WL_CONNECTED)){
    Serial.print('.');
    delay(1000);
  }
  
  // The wifi is not connected return 0
  if(WiFi.status() != WL_CONNECTED){
    returnStatus = false;
    Serial.println("Not Connected");
  }
  else{ // The wifi is connected return 1
    Serial.println(WiFi.localIP());
    Serial.println("Connected");
    returnStatus = true;
  }

  return returnStatus;
}

void setup() {
    Serial.begin(115200);
    M5.begin();
    M5.Imu.Init();

    bool connectedToWifi = connectToEduroam(EAP_IDENTITY,EAP_PASSWORD);
    Serial.println(connectedToWifi);

    // Calculates the amount of units we are measuring:
    numOfUnits = sizeof(unitArray) / sizeof(unitArray[0]);

    bootMQTT();  //starts mqtt service if needed

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    startTime = Get_Epoch_Time();
    double tempEpoch = Get_Epoch_Time();
    while (startTime == tempEpoch) {
        startTime = Get_Epoch_Time();
    }
    startMil = millis();

    // Create a thread on core 0
    xTaskCreatePinnedToCore(
        Task1code,  // Task function
        "Task1",    // Task name
        10000,      // Stack size
        NULL,       // Task parameters
        1,          // Task priority
        &Task1,     // Task handle
        0           // Core ID (0 or 1)
    );
  
}

void sendMQTT(String inputString) {  
  if (WiFi.status() == WL_CONNECTED) {
    if(client.connected() == true){
      const char* c = inputString.c_str();
      //inputString.toCharArray(outChar, inputString.length() + 1);
      client.publish(topic, c);
      Serial.println(inputString);
    }
    else{
      Serial.println("Broker Disconnected. Restarting ESP");
      ESP.restart();
    }
  }
  else {
    Serial.println("WiFi Disconnected. Restarting ESP");
    ESP.restart();
  }

}

void debugStatement(String officalStart, String officalEnd){
  Serial.print("Sending an MQTT Packet with the start time: ");
  Serial.print(officalStart);
  Serial.print(" and the end time: ");
  Serial.println(officalEnd);
}

void loop() {
  if(finishCollection == true){
    Serial.println("-----------------------------------");
    debugStatement(officalStart, officalEnd);
    sendMQTT(createOutMQTT(officalStart, officalEnd, rowData.accZArray, macAddress, unitArray[2]));

    finishCollection = false;
  }
  
}
