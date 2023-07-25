#include <WiFi.h> //Wifi library
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks

//Identity for user with password related to his realm (organization)

bool connectedToWifi = false;
bool STAWifiConnected = false;

char* STAusername;
char* STApassword;

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
  
  
  // Loop for 10 seconds to conect to the network
  while ((WiFi.status() != WL_CONNECTED)){
    Serial.print('.');
    delay(1000);
  }
  
  // The wifi is not connected return 0
  if(WiFi.status() != WL_CONNECTED){
    STAWifiConnected = false;
    returnStatus = false;
    Serial.println("Not Connected");
  }
  else{ // The wifi is connected return 1
    Serial.println(WiFi.localIP());
    Serial.println("Connected");
    returnStatus = true;
    STAWifiConnected = true;
  }

  return returnStatus;
}

void setup() {
  Serial.begin(115200);
  // String EAP_IDENTITY  = "noahro@bu.edu";
  // String EAP_PASSWORD  = "MonsterMonster()";

  char* EAP_IDENTITY  = "noahro@bu.edu";
  char* EAP_PASSWORD  = "MonsterMonster()";

  connectedToWifi = connectToEduroam(EAP_IDENTITY,EAP_PASSWORD);
  Serial.println(connectedToWifi);

}

void loop() {
  yield();
}
