#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <LittleFS.h>

WiFiManager wm;
const char * nomDeFichier = "/index.html";  // <<--- NOM DU FICHIER HTML DE LA PAGE WEB

const byte pinLed = D1;

// Initialisation des Pin Entrés et Sortie de l'ESP
void InitPin() {
  pinMode(pinLed, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  digitalWrite(LED_BUILTIN, LOW);
}

// Crée un serveur sur le port HTTP standard (80)
const uint16_t HTTPPort = 80;
WiFiServer serveurWeb(HTTPPort);

const byte maxHTTPLine = 100;
char httpLine[maxHTTPLine + 1];          // +1 pour avoir la place du '\0'

const byte maxURL = 50;
char urlRequest[maxURL + 1];             // +1 pour avoir la place du '\0'

// Initialisation du WiFi + serveur Web
void InitWiFi() {

  // Si il n'y a pas de donner de WiFi dans la mémoire flash, on crée un Wifi pour entrés les information
  WiFi.mode(WIFI_STA);

  if (!wm.autoConnect("BotBar")) {
    Serial.println("Erreur de connexion.");
  } else {
    Serial.println("Connexion etablie !");
  }

  // Démarrage Serveur Web
  serveurWeb.begin();

  // Donne l'ip de Connexion au réseaux dans le Terminal
  Serial.print(F("Site web http://")); Serial.print(WiFi.localIP());
  if (HTTPPort != 80) {
    Serial.print(F(":"));
    Serial.print(HTTPPort);
  }
  Serial.println();
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

// Fonction RelayRhum(cl) est utiliser pour allumer
// le Relay lors d'un appuis sur le bouton de la page WEB
// ou grace a l'application
void RelayRhum(WiFiClient &cl) {

  digitalWrite(pinLed, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  cl.println("En cours");
  delay(2000);
  digitalWrite(pinLed, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  cl.println("Terminer");

}

// Gestion des requette HTTP
boolean testRequeteWeb() {
  boolean requeteHTTPRecue = false;
  byte indexMessage = 0;
  char * ptrGET, *ptrEspace;

  WiFiClient client = serveurWeb.available();

  //on vérifie si des client son connecter au WiFi
  if (!client) return requeteHTTPRecue;

  boolean currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read(); 

      // une requête HTTP se termine par une ligne vide
      if (c == '\n' && currentLineIsBlank) {

        // Ceci n'est pas pour le favicon du site Web
        if (strcmp(urlRequest, "/favicon.ico")) {
          requeteHTTPRecue = true;

          // On envoie un en tête de réponse HTTP standard
          client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n");

          // On regarde si on a une requete qui continet "/reqEtatBouton"
          // si c'est le cas, on appelle la fonction RelayRhum() qui allume le relay
          if (strstr(urlRequest, "/reqEtatBouton")) {
            RelayRhum(client);
          }

          // On regarde si on a une requete qui continet "/connexion"
          // si c'est le cas, elle permet la validation de l'adresse ip sur l'application
          if (strstr(urlRequest, "/connexion")) {
            client.println("connexion OK");
          }

          /* Affichage de la page Web (pas utiliser pour le moment)
          
            if (LittleFS.exists(nomDeFichier)) {
              File pageWeb = LittleFS.open(nomDeFichier, "r");
              client.write(pageWeb);
              pageWeb.close();
            } else {
              Serial.println(F("Erreur de fichier"));
            }

          */
            
        }
        break;
      } // fin de génération de la réponse HTTP

      if (c == '\n') {
        currentLineIsBlank = true;
        httpLine[indexMessage] = '\0';             // on termine la ligne correctement (c-string)
        indexMessage = 0;                          // on se reprépre pour la prochaine ligne
        if (ptrGET = strstr(httpLine, "GET")) {
          // c'est la requête GET, la ligne continent "GET /URL HTTP/1.1", on extrait l'URL
          ptrEspace = strstr(ptrGET + 4, " ");
          *ptrEspace = '\0';
          strncpy(urlRequest, ptrGET + 4, maxURL);
          urlRequest[maxURL] = '\0';               // par précaution si URL trop longue
        }
      } else if (c != '\r') {
        currentLineIsBlank = false;
        if (indexMessage <= maxHTTPLine - 1) {
          httpLine[indexMessage++] =  c;           // sinon on ignore le reste de la ligne
        }
      }
    }   // end if available
  }     // end while
  delay(1);
  client.stop(); // termine la connexion
  return requeteHTTPRecue;
}

void setup() {
  // Vitesse de la communication Série 9600 Bit/s
  Serial.begin(9600);

  // Initialisation des Pin Entrés est Sortie de l'ESP
  InitPin();

  //démarrage de LittleFS (Gestionnaire de fichier) pour la page HTML
  InitLittleFS();

  // Apelle de la fonction InitWiFi() pour démaré la 
  // Comunication Entre le Wifi est le Programme
  InitWiFi();
}

void loop() {
  testRequeteWeb();
}