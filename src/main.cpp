#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <LittleFS.h>

const char * nomDeFichier = "/Hello.html";  // <<--- NOM DU FICHIER HTML DE LA PAGE WEB

const char* ssid = "SSID";          // <<--- METTRE ICI VOTRE NOM RESEAU WIFI
const char* password = "MDP";       // <<--- METTRE ICI VOTRE MOT DE PASSE WIFI

//Ouverture du port http pour la page web
const uint16_t HTTPPort = 80;
WiFiServer serveurWeb(HTTPPort);


//Récupération et inscription dans le terminale, l'ip de l'ESP
void printHTTPServerInfo() {

  Serial.print(F("Site web http://")); Serial.print(WiFi.localIP());
  if (HTTPPort != 80) {
    
    Serial.print(F(":"));
    Serial.print(HTTPPort);
  }
  Serial.println();

}

//Génération de la page web grace au fichier html dans la mémoire flash
void testRequeteWeb() {

  boolean currentLineIsBlank = true;

  WiFiClient client = serveurWeb.available();
  if (!client) return; // pas de client connecté

  while (client.connected()) {
    if (client.available()) {
      // on lit toute la trame HTPP, ici sans se soucier de la reqête
      char c = client.read();

      if (c == '\n' && currentLineIsBlank) { // une requête HTTP se termine par une ligne vide
        // ON GENERE LA PAGE WEB
        // On envoie un en tête de réponse HTTP standard
        client.println(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"));
        
        if (LittleFS.exists(nomDeFichier)) {
          File pageWeb = LittleFS.open(nomDeFichier, "r");
          client.write(pageWeb);
          pageWeb.close();
        } else {
          Serial.println(F("Erreur de fichier"));
        }
        // on sort du while et termine la requête
        break;
      }
      if (c == '\n') currentLineIsBlank = true;
      else if (c != '\r') currentLineIsBlank = false;
    } // end if available
  } // end while
  delay(1);
  client.stop(); // termine la connexion

}


void setup() {
  Serial.begin(9600); //ouverture communication série a 9600bit/s
  Serial.println("\n\nTest SPIFFS\n");

  // on démarre le SPIFSS
  if (!LittleFS.begin()) {
    Serial.println("erreur SPIFFS");
    while (true); // on ne va pas plus loin
  }

  //connection au WiFi
  WiFi.begin(ssid, password);

  //donne le statue de connexion au WiFi
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.write('.');
  }
  Serial.println();

  // on démarre le serveur
  serveurWeb.begin();
  printHTTPServerInfo();

}

//Démarrage de la fonction testRequeteWeb() pour la création de la page Web
void loop() {
  testRequeteWeb();
}