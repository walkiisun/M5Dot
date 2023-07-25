/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

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

// Function to connect to the standard network

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
}

void setup() {

  Serial.begin(115200);

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

  // Serial.print("The access to internet is logic: ");  
  // Serial.println(connectedToWifi);

  if(!STAWifiConnected){
    Serial.println("I am now starting the webserver");
    initPages();
  }

  // WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

// void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
//   Serial.println("Disconnected from WiFi access point");
//   Serial.print("WiFi lost connection. Reason: ");
//   Serial.println(info.wifi_sta_disconnected.reason);
//   Serial.println();
//   Serial.println("there has been a disconnection");
//   unsigned long endTime = millis() + 10000;
//   unsigned long currentTime = millis();
//   if(persistantMode){
//     if(currentTime < endTime){
//       if(strcmp(params[0],"EAP") == 0){
//         Serial.println("Call EAP Function with: username and password: ");
//         connectToEduroam(params[2],params[3]);
//       }
//       if(strcmp(params[0],"WPA2") == 0){
//         Serial.println("Call WPA2 Function with: username and password: ");
//       connectToWifi(params[1],params[3],false);
//       }  
//       currentTime = millis();
//     }
//   }else{
//     initPages();
//     STAWifiConnected = false;
//   }
// }
void Task1code(void* pvParameters) {
  for (;;) {
    if(count == 100){count = 0;}
    count++;
    Serial.println(count);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


void loop() {
  WiFi.status();
  Serial.print("hello, the value of params[0] is: ");
  Serial.println(params[0]);

  if(inputFlag){ // There is a new user response, please check
    if(strcmp(params[0],"EAP") == 0){
      Serial.println("Call EAP Function with: username and password: ");
      connectToEduroam(params[2],params[3]);

    }
    if(strcmp(params[0],"WPA2") == 0){
      Serial.println("Call WPA2 Function with: username and password: ");
      connectToWifi(params[1],params[3],false);
    }
    if(!STAWifiConnected){initPages();}
    inputFlag = false;
  }
  if(count == 100){
    Serial.println("Count is 100!");
    delay(2000);
  }

  delay(500);

}