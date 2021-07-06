#include "DHT.h"
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#define FIREBASE_HOST "smarthome-ba53b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "x8zpX7a9HCu3hAUEmDRVuzv8bBCk79nKhQvkXmZW"

#define WIFI_NETWORK "QUANG"
#define WIFI_PASSWORD "123456789vu"
FirebaseData firebaseData;
AsyncWebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int pinSoundSS = 25;
int pinGasSS = 14;
int statusSSS = 0;
int echo = 26;
int trig = 27;
unsigned long timeOn = 0;
unsigned long timeOff = 0;

unsigned long  premillis = 0;
void setup() {

  pinMode(pinGasSS, INPUT);
  pinMode(pinSoundSS, INPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  dht.begin();
  Serial.println("[WIFI] Connecting");
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 0 );
  lcd.print("   Smart Home   ");
  lcd.setCursor(2, 1 );
  lcd.print(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  xTaskCreate(soundSensor, "sound", 30720, NULL, 1, NULL);

  if (!SPIFFS.begin(true)) {
    Serial.println("Error SPIFFS");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "");
  });
  server.begin();
}
void SoundSensor() {
  unsigned long  currentmillis;
  if (digitalRead(pinSoundSS) == 0) {
    premillis = millis();
    vTaskDelay(300);
    currentmillis = millis();
    unsigned long  tmp = currentmillis - premillis;
    while ((tmp >= 300) && (tmp <= 1000)) {
      if (digitalRead(pinSoundSS) == 0) {
        if (statusSSS == 1) {
          Serial.println("Status SS: on");
          Firebase.setString(firebaseData, "/Lamp/bedRoom", "ON");
          statusSSS = 0;
        }
        else {
          Serial.println("Status SS: off");
          Firebase.setString(firebaseData, "/Lamp/bedRoom", "OFF");
          statusSSS = 1;
        }
        break;
      }
      currentmillis = millis();
      tmp = currentmillis - premillis;
    }
  }
}
void UtrasonicSensor() {
  unsigned long duration;
  int distance;
  digitalWrite(trig, 0);
  delay(2);
  digitalWrite(trig, 1);
  delay(5);
  digitalWrite(trig, 0);

  duration = pulseIn(echo, HIGH);

  distance = int(duration / 2 / 27);
  Serial.print(distance);
  Serial.println("cm");
  if (distance < 10) {
    timeOn = millis();
    Firebase.setString(firebaseData, "Lamp/toilet", "ON");
  } else  timeOff = millis();
  delay(700);

  if ((timeOff - timeOn) > 60000 && (timeOff - timeOn) < 65000) {
    Firebase.setString(firebaseData, "Lamp/toilet", "OFF");
  }
}

void loop() {
  //  //  SoundSensor();
  UtrasonicSensor();

}
void soundSensor(void *parameter) {
  (void) parameter;
  for (;;) {
    SoundSensor();
  }
}
