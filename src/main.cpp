#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <LittleFS.h>
#include <ConfWiFi.h>

const byte pinLed = D1;

// Initialisation des Pin Entrés et Sortie de l'ESP
void InitPin() {
  pinMode(pinLed, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

//démarrage de LittleFS (Gestionnaire de fichier) pour la page HTML
void InitLittleFS() {
  Serial.println("\n\nTest LittleFS\n");

  // Si il y a une erreur, on ne vas pas plus loins
  if (!LittleFS.begin()) {
    Serial.println("erreur LittleFS");
    while (true);
  }
}

void setup() {
  // Vitesse de la communication Série 9600 Bit/s
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // Initialisation des Pin Entrés est Sortie de l'ESP
  InitPin();

  //démarrage de LittleFS (Gestionnaire de fichier) pour la page HTML
  InitLittleFS();

  // Apelle de la fonction InitWiFi() pour démaré la 
  // Comunication Entre le Wifi est le Programme
  initWiFi(LED_BUILTIN, "ESP", "");
}

void loop() {

}