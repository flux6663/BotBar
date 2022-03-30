#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include "LittleFS.h"
#include "ConfWiFi.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

//Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;

IPAddress localIP;
IPAddress localGateway;
IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

// Gestion LED
int ledPin;
bool ledState = LOW;

// Initialize WiFi
void initWiFi(int led_create, String ssid_create, String password_create) {
    ssid = readFile(LittleFS, ssidPath);
    pass = readFile(LittleFS, passPath);
    ip = readFile(LittleFS, ipPath);
    gateway = readFile (LittleFS, gatewayPath);
    Serial.println(ssid);
    Serial.println(pass);
    Serial.println(ip);
    Serial.println(gateway);

    ledPin = led_create;

    if(!CreateWiFi()) {

        // Connect to Wi-Fi network with SSID and password
        Serial.println("Setting AP (Access Point)");
        // NULL sets an open Access Point

        WiFi.softAP(ssid_create, password_create);

        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP); 

        // Web Server Root URL
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/wifimanager.html", "text/html");
        });
        
        server.serveStatic("/", LittleFS, "/");
        
        server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
        int params = request->params();
        for(int i=0;i<params;i++){
            AsyncWebParameter* p = request->getParam(i);
            if(p->isPost()){
            // HTTP POST ssid value
            if (p->name() == PARAM_INPUT_1) {
                ssid = p->value().c_str();
                Serial.print("SSID set to: ");
                Serial.println(ssid);
                // Write file to save value
                writeFile(LittleFS, ssidPath, ssid.c_str());
            }
            // HTTP POST pass value
            if (p->name() == PARAM_INPUT_2) {
                pass = p->value().c_str();
                Serial.print("Password set to: ");
                Serial.println(pass);
                // Write file to save value
                writeFile(LittleFS, passPath, pass.c_str());
            }
            // HTTP POST ip value
            if (p->name() == PARAM_INPUT_3) {
                ip = p->value().c_str();
                Serial.print("IP Address set to: ");
                Serial.println(ip);
                // Write file to save value
                writeFile(LittleFS, ipPath, ip.c_str());
            }
            // HTTP POST gateway value
            if (p->name() == PARAM_INPUT_4) {
                gateway = p->value().c_str();
                Serial.print("Gateway set to: ");
                Serial.println(gateway);
                // Write file to save value
                writeFile(LittleFS, gatewayPath, gateway.c_str());
            }
            //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
            }
        }
        ESP.restart();
        request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
        });
        server.begin();
    }

}

// Create WiFi
bool CreateWiFi() {

  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());

  if (!WiFi.config(localIP, localGateway, subnet)){
    Serial.println("STA Failed to configure");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());

  Serial.println("Connecting to WiFi");
    
    int i = 0;
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(800);
        Serial.print(".");
        
        // switch led statue
        ledState = !ledState;
        digitalWrite(ledPin, !ledState);

        if (i > 20) {
            Serial.println("");
            Serial.println("WiFi connection failed");
            return false;
        }
        
        i++;
    }

  Serial.println(WiFi.localIP());

    digitalWrite(ledPin, LOW);
    delay(2000);
    digitalWrite(ledPin, HIGH);

  return true;
}

// Read File from LittleFS
String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

// Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
  file.close();
}