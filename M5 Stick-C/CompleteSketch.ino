/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
#include <M5StickCPlus.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "time.h"
// #include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks


AsyncWebServer server(80);

// Global Variables
String inputSSID;
String inputUsername = "NULL";
String inputPassword;
String inputParam = "NULL";

char *params[] = { "inputPram", "inputSSID", "inputUsername", "inputPassword" };


// // REPLACE WITH YOUR NETWORK CREDENTIALS
char* STAssid;
char* STAusername;
char* STApassword;

// Flags
bool STAWifiConnected = false;
bool inputFlag = false;
bool persistantMode = false;
bool disconnectionError = false;

// Core Test:
int count = 0;
TaskHandle_t Task1;


// Data Collection //
//imu vars

float pitch = 0.0F;
float roll = 0.0F;
float yaw = 0.0F;

//String vars
String macAddress = WiFi.macAddress();  //"00:00:00:00:00:00";

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
  float rollArray[numDataPoints];
  // float accXArray[numDataPoints];
  // float accYArray[numDataPoints];
  // float accZArray[numDataPoints];
};

RowData rowData;

String unitArray[] = {
  "roll",
  // "accY",
  // "accZ",
};

int numOfUnits;
bool finishCollection = false;
bool initDataCollection = false;
int samplePeriord = 10;
int timeToSendData = false;

//epoch vars

String numString;
String packetStartEpoch;
String packetStopEpoch;


//mqtt vars 
const char* mqtt_broker = "172.21.127.197";
const char* topic = "RollDemo";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

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
    M5.IMU.getAhrsData(&pitch, &roll, &yaw);
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

    rowData.rollArray[count] = roll;
    // rowData.accZArray[count] = accZ;
    // rowData.accYArray[count] = accY;

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


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>WPA2<h3>
  <form action="/get ">
    SSID: <input type="text" name="WPASSID"><br>
    Password: <input type="text" name="WPAPassword">
    <input type="submit" value="Submit">
  </form><br>
  <h3>EAP<h3>
  <form action="/get">
    SSID: <input type="text" name="EAPSSID"><br>
    Username: <input type="text" name="EAPUsername"><br>
    Password: <input type="password" name="EAPPassword">    
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


//connectedToWifi = connectToWifi("notSure","notSure", true);
void connectToWifi(String InputSSID, String InputPassword, bool firstTime){
  // If it is the first time, check the username and password in storage and see if the last known information is correct
  // In this case, default is sensorweb informatoin
  STAssid = &InputSSID[0];
  STApassword = &InputPassword[0];

  // If this is the fist time enter this init loop
  if(firstTime){
    Serial.println("starting with default settings");
    InputSSID = "sensorweb";
    //InputPassword = "sensorweb128";
    InputPassword = "Incorrect Password";


    STAssid = &InputSSID[0];
    STApassword = &InputPassword[0];
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(STAssid, STApassword);
  Serial.print("Connecting to WiFi ..");

  // Loop for 10 seconds to conect to the network
  unsigned long endTime = millis() + 10000;
  unsigned long currentTime = millis();
  while ((WiFi.status() != WL_CONNECTED) && (currentTime < endTime)){
    Serial.print('.');
    delay(1000);
    currentTime = millis();
  }
  
  // The wifi is not connected return 0
  if(WiFi.status() != WL_CONNECTED){
    STAWifiConnected = false;
    Serial.println("Not Connected");
  }
  else{ // The wifi is connected return 1
    Serial.println(WiFi.localIP());
    Serial.println("Connected");

    STAWifiConnected = true;
    disconnectionError = false;
  }
  firstTime = false;
}

void connectToEduroam(String InputUsername, String InputPassword) {
  char* ssid = "eduroam"; // eduroam SSID


  STAusername = &InputUsername[0];
  STApassword = &InputPassword[0];

  delay(10);
  WiFi.mode(WIFI_STA); //init wifi mode
  WiFi.begin(ssid, WPA2_AUTH_PEAP, STAusername, STAusername, STApassword); //without CERTIFICATE
  Serial.print(("Connecting to network: "));
  Serial.println(ssid);
  
  
  // Loop for 10 seconds to conect to the network
  unsigned long endTime = millis() + 10000;
  unsigned long currentTime = millis();
  while ((WiFi.status() != WL_CONNECTED) && (currentTime < endTime)){
    Serial.print('.');
    delay(1000);
    currentTime = millis();
  }
  
  // The wifi is not connected return 0
  if(WiFi.status() != WL_CONNECTED){
    STAWifiConnected = false;
    Serial.println("Not Connected");
  }
  else{ // The wifi is connected return 1
    Serial.println(WiFi.localIP());
    Serial.println("Connected");
    
    STAWifiConnected = true;
    disconnectionError = false;
  }
}

void connectAP(){
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {

    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam("WPASSID")) {
      inputSSID = request->getParam("WPASSID")->value();
      inputPassword = request->getParam("WPAPassword")->value();

      inputUsername = "NULL";
      inputParam = "WPA2";

    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam("EAPSSID")) {
      inputSSID = request->getParam("EAPSSID")->value();
      inputUsername = request->getParam("EAPUsername")->value();
      inputPassword = request->getParam("EAPPassword")->value();

      inputParam = "EAP";
    } else {
      inputSSID = "No message sent";
      inputUsername = "NULL";
      inputParam = "none";
    }

    // Change the character types of the varibales to char*
    params[0] = &inputParam[0];
    params[1] = &inputSSID[0];
    params[2] = &inputUsername[0];
    params[3] = &inputPassword[0];

    // Print the variables
    Serial.println(params[0]);
    Serial.println(params[1]);
    Serial.println(params[2]); 
    //Serial.println(params[3]);

    inputFlag = true;

    // Return Message
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputSSID +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });

  server.onNotFound(notFound);
  server.begin();
}

// Function to start the soft access point
void startSoftAP(){
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  String macAddress = WiFi.macAddress();

  // Create Soft AP Name
  String last5Characters = macAddress.substring(macAddress.length() - 5);
  last5Characters.replace(":", "");

  // Configure the Access Points Name 
  String softAP = "ESP " + last5Characters;
  WiFi.softAP(softAP);

  // Configure the ESP32 access point
  IPAddress apIP(10, 0, 0, 1); // Set the IP address of the ESP32 access point
  IPAddress netMsk(255, 255, 255, 0); // Set the subnet mask of the ESP32 access point
  
  WiFi.softAPConfig(apIP, apIP, netMsk);
  IPAddress retrievedNetMsk = WiFi.softAPIP();
  Serial.print("Access Point Subnet Mask: ");
  Serial.println(retrievedNetMsk);

  // Print IP Address
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

void initPages(){
  startSoftAP(); // Start the SoftAP
  connectAP(); //Start the web server
  params[0] = "inputParam";
  timeToSendData = false;
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

// This function is called from the main loop and will check to see if the data collection is complete
void loopCall(){
   if(finishCollection == true){
    Serial.println("-----------------------------------");
    int delayTime = 1000;
    debugStatement(officalStart, officalEnd);

    sendMQTT(createOutMQTT(officalStart, officalEnd, rowData.rollArray, macAddress, unitArray[0]));

    finishCollection = false;
  }
}

void configTime(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.print("2");
  startTime = Get_Epoch_Time();
  double tempEpoch = Get_Epoch_Time();
  while (startTime == tempEpoch) {
      startTime = Get_Epoch_Time();
  }
  startMil = millis();
}


void setup() {

  Serial.begin(115200);
  Serial.println("Hello this is the begining");
  M5.begin();
  M5.Imu.Init();

  numOfUnits = sizeof(unitArray) / sizeof(unitArray[0]);

  // Create Task1 on core 0
  xTaskCreatePinnedToCore(
      Task1code,     // Task function
      "Task1",       // Task name
      10000,         // Stack size
      NULL,          // Task parameters
      1,             // Task priority
      &Task1,        // Task handle
      0              // Core ID (0 or 1)
  );

  // Check the connection, if you can connect return true after 5 seconds, else return false
  connectToWifi("notSure","notSure", true);

  if(!STAWifiConnected){
    Serial.println("I am now starting the webserver");
    initPages();
  }
}


void loop() {
  if(!timeToSendData){
      // WiFi.status();
      Serial.print("hello, the value of params[0] is: ");
      Serial.println(params[0]);
      Serial.print("The Input Flag is:");
      Serial.println(inputFlag);
    if(inputFlag){ // There is a new user response, please check
      if(strcmp(params[0],"EAP") == 0){
        Serial.println("Call EAP Function with: username and password: ");
        connectToEduroam(params[2],params[3]);

      }
      if(strcmp(params[0],"WPA2") == 0){
        Serial.println("Call WPA2 Function with: username and password: ");
        connectToWifi(params[1],params[3],false);
      }      
      timeToSendData = true;
      inputFlag = false;
      initDataCollection = true;

      if(!STAWifiConnected){initPages();}
      else{bootMQTT();}  //starts mqtt service if needed }
    }
    delay(1000);

  }else{
    if(initDataCollection){configTime();}
    loopCall();
    initDataCollection = false;
  }
}




