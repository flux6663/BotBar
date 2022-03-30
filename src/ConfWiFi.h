// Initialisation du réseau Wi-Fi
void initWiFi(int led_create, String ssid_create, String password_create);

// Création du réseaux WiFI
bool CreateWiFi();

// Gestion Fichier
String readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);

// ESP restart
void ESPRestart();