#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>
#include <vector>

#define DNS_PORT 53
#define NUM_DRINKS 16
#define NUM_INGREDIENTS 8

#define NUM_LEDS 24       // Le nombre de LEDs sur votre bande
#define PIN 4             // Le pin sur lequel votre bande est connectée


IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
AsyncWebServer server(80);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int8_t Demande_Boisson;
bool boisson = false;

//Define your relay GPIOs
const int relay[NUM_INGREDIENTS] = {32, 33, 25, 26, 27, 14, 23, 22};

struct Ingredient {
    int pump;
    long duration;
};

struct Drink {
    String name;
    std::vector<Ingredient> ingredients;
    unsigned long totalDuration;  // Ajouté
    unsigned long drinkStartTime;  // Ajouté
};

struct ActiveDrink {
  Drink drink;
  int currentIngredientIndex;
  unsigned long startTime;
};

// Définir les ingrédients pour chaque boisson
std::vector<Ingredient> ingredients_menthe_a_l_eau = {{27, 12000}, {32, 48000}};
std::vector<Ingredient> ingredients_grenadine_a_l_eau = {{14, 12000}, {32, 48000}};
std::vector<Ingredient> ingredients_diabolo_grenadine = {{23, 48000}, {14, 12000}};
std::vector<Ingredient> ingredients_diabolo_menthe = {{23, 48000}, {27, 12000}};
std::vector<Ingredient> ingredients_jus_de_fruit = {{32, 36000}, {33, 36000}, {25, 24000}, {26, 12000}};
std::vector<Ingredient> ingredients_special_botbar = {{32, 36000}, {33, 36000}, {25, 24000}, {26, 12000}, {27, 12000}, {14, 12000}, {23, 24000}, {22, 12000}};
std::vector<Ingredient> ingredients_afterglow = {{33, 36000}, {14, 12000}, {22, 12000}};
std::vector<Ingredient> ingredients_lemon = {{25, 24000}, {23, 48000}};
std::vector<Ingredient> ingredients_danies = {{32, 36000}, {25, 24000}, {23, 48000}};
std::vector<Ingredient> ingredients_borabora = {{33, 36000}, {25, 24000}, {26, 12000}, {23, 48000}};
std::vector<Ingredient> ingredients_bella_luna = {{32, 36000}, {33, 36000}, {23, 48000}};
std::vector<Ingredient> ingredients_mojito = {{25, 24000}, {27, 12000}, {23, 48000}};
std::vector<Ingredient> ingredients_splendide_beaute = {{32, 36000}, {14, 12000}, {23, 48000}};
std::vector<Ingredient> ingredients_letitia_special = {{33, 36000}, {27, 12000}, {22, 12000}};
std::vector<Ingredient> ingredients_sex_on_the_beach = {{32, 36000}, {33, 36000}, {14, 12000}, {23, 48000}};
std::vector<Ingredient> ingredients_pina_colada = {{33, 36000}, {22, 12000}};

// Définir vos boissons ici
std::vector<Drink> drinks = {
    { "Menthe à l'eau", ingredients_menthe_a_l_eau },
    { "Grenadine à l'eau", ingredients_grenadine_a_l_eau },
    { "Diabolo Grenadine", ingredients_diabolo_grenadine },
    { "Diabolo Menthe", ingredients_diabolo_menthe },
    { "100% Jus de fruit", ingredients_jus_de_fruit },
    { "Spécial BotBar", ingredients_special_botbar },
    { "Afterglow", ingredients_afterglow },
    { "Lemon", ingredients_lemon },
    { "Danies", ingredients_danies },
    { "BoraBora", ingredients_borabora },
    { "Bella Luna", ingredients_bella_luna },
    { "Mojito", ingredients_mojito },
    { "Splendide Beaute", ingredients_splendide_beaute },
    { "Letitia Special", ingredients_letitia_special },
    { "Sex on the beach", ingredients_sex_on_the_beach },
    { "Piña Colada", ingredients_pina_colada }
};

ActiveDrink* activeDrink = nullptr;

void startMixing(Drink drink) {
  // Calculer la durée totale de la boisson
  unsigned long totalDuration = 0;
  for (const auto& ingredient : drink.ingredients) {
      totalDuration += ingredient.duration;
  }
  drink.totalDuration = totalDuration;

  // Enregistrer le moment où nous avons commencé à préparer cette boisson
  drink.drinkStartTime = millis();

  activeDrink = new ActiveDrink { drink, 0, millis() };
  digitalWrite(drink.ingredients[0].pump, LOW);
}

void handleRequest(AsyncWebServerRequest *request) {
    if (request->hasParam("drink")) {
        String drinkName = request->getParam("drink")->value();
        auto it = std::find_if(drinks.begin(), drinks.end(), [&drinkName](const Drink& drink) {
            return drink.name == drinkName;
        });
        if (it != drinks.end()) {
            request->send(200, "text/plain", "Mélange de " + drinkName);
            Serial.println("Mélange de " + drinkName);
            if (activeDrink != nullptr) {
            delete activeDrink;
            }
            startMixing(*it);
            return;
        }
        request->send(404, "text/plain", "Boisson introuvable");
        Serial.println("Boisson introuvable");
    } else {
        request->send(400, "text/plain", "Paramètre 'drink' manquant");
        Serial.println("Paramètre 'drink' manquant");
    }
}

uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Cette fonction crée un effet d'arc-en-ciel
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void setup() {
    //Setup Relay GPIOs
    for (int i=0; i<8; i++) {
        pinMode(relay[i], OUTPUT);
        digitalWrite(relay[i], HIGH);
    }
    Serial.begin(115200);

    // Start WiFi
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("BotBar");

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "botbar.fr", apIP);

    // Gestionnaire d'URL pour la requête POST
    server.on("/api/mix", HTTP_POST, handleRequest);

    server.begin();

    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
}

void loop() {
    dnsServer.processNextRequest();

    if (activeDrink != nullptr) {
        Ingredient& currentIngredient = activeDrink->drink.ingredients[activeDrink->currentIngredientIndex];
        if (millis() - activeDrink->startTime >= currentIngredient.duration) {
            digitalWrite(currentIngredient.pump, HIGH);
            activeDrink->currentIngredientIndex++;
            if (activeDrink->currentIngredientIndex >= activeDrink->drink.ingredients.size()) {
                delete activeDrink;
                activeDrink = nullptr;
            } else {
                currentIngredient = activeDrink->drink.ingredients[activeDrink->currentIngredientIndex];
                digitalWrite(currentIngredient.pump, LOW);
                activeDrink->startTime = millis();
            }

            
        }

        // Gestion des LEDs seulement si activeDrink n'est pas nullptr
        if (activeDrink != nullptr) {
            unsigned long elapsed = millis() - activeDrink->drink.drinkStartTime;
            unsigned long totalDuration = activeDrink->drink.totalDuration;
            int numActiveLEDs = NUM_LEDS * (totalDuration - elapsed) / totalDuration;
            for (int i = 0; i < numActiveLEDs; i++) {
                strip.setPixelColor(i, Wheel((i + millis() / 100) % 256)); // Scroll through the color wheel
            }
            for (int i = numActiveLEDs; i < NUM_LEDS; i++) {
                strip.setPixelColor(i, 0); // Turn off the remaining LEDs
            }
            strip.show();
        }
    }


}