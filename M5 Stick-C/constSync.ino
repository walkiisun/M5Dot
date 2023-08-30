#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Int64String.h>
#include <ESPNtpClient.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks


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



//String vars
String macAddress = WiFi.macAddress();  //"00:00:00:00:00:00";


TaskHandle_t Task1;

//time vars


int64_t curMil;

String officalStart;
String officalEnd;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -18000;
const int daylightOffset_sec = 0;

const int numDataPoints = 100;

boolean syncEventTriggered = false; // True if a time even has been triggered
NTPEvent_t ntpEvent; // Last triggered event


struct RowData {
  float rollArray[numDataPoints];
  float pitchArray[numDataPoints];
  float yawArray[numDataPoints];
};

RowData rowData;

String unitArray[] = {
  "roll",
  "pitch",
  "yaw",
};

int numOfUnits;
int count;
bool finishCollection = false;


int samplePeriord = 10; // 10 ms == 100hz

//epoch vars

String numString;
String packetStartEpoch;
String packetStopEpoch;


WiFiClient espClient;
PubSubClient client(espClient);

//mqtt vars 
const char* mqtt_broker = "10.250.69.252";
const char* topic = "shake";
const int mqtt_port = 1883;

void Task1code(void* pvParameters) {
  for (;;) {
    getDataIMU();
    collectData();
  }
}

String epochString() {
  numString = int64String(NTP.millis());
  return numString;
}

void ntpSync(){
  NTP.setTimeZone(TZ_Etc_UTC);
  NTP.onNTPSyncEvent([] (NTPEvent_t event) {ntpEvent = event; syncEventTriggered = true;});
  NTP.setInterval(150);
  NTP.begin(ntpServer, false);
}

void getDataIMU() {
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  M5.IMU.getAhrsData(&pitch, &roll, &yaw);
}


String createOutMQTT(String db_name, String table_name, String data_name, float* dataArray, String macAddress, String start_timestamp, int interval) {
  String result = db_name + " ";
  result += table_name + " ";
  result += data_name + " ";
  for (int i = 0; i < numDataPoints; i++) {
    result += String(dataArray[i]);
    if (i < numDataPoints - 1) {
      result += ",";
    }
  }
  result += " " + macAddress + " ";  
  result += start_timestamp + " ";
  result += interval;
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
  if(count < numDataPoints-1){
    if(count == 0){packetStartEpoch = epochString();} 

    
    rowData.rollArray[count] = roll;
    rowData.pitchArray[count] = pitch;
    rowData.yawArray[count] = yaw;
    
    

    vTaskDelay(samplePeriord);
    count++;
  } 
  else if(count == numDataPoints - 1){
    rowData.rollArray[count] = roll;
    rowData.pitchArray[count] = pitch;
    rowData.yawArray[count] = yaw;
    count++;
  }
  else{ // The last index of the array
    packetStopEpoch = epochString();

    officalEnd = packetStopEpoch;
    officalStart = packetStartEpoch;

    count = 0;
    finishCollection = true;
  }
  
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

    
    
    ntpSync();

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
    int delayTime = 250;
    debugStatement(officalStart, officalEnd);

    // (String db_name, String table_name, String data_name, float* dataArray, String macAddress, String start_timestamp, int interval)
    sendMQTT(createOutMQTT("shake", "roll", "value", rowData.rollArray, macAddress, officalStart, samplePeriord));
    delay(delayTime);

    sendMQTT(createOutMQTT("shake", "pitch", "value", rowData.pitchArray, macAddress, officalStart, samplePeriord));
    delay(delayTime);

    sendMQTT(createOutMQTT("shake", "yaw", "value", rowData.yawArray, macAddress, officalStart, samplePeriord));
    


    finishCollection = false;
  }
  
}
