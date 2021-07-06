#include "DHT.h"
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define FIREBASE_HOST "smarthome-ba53b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "x8zpX7a9HCu3hAUEmDRVuzv8bBCk79nKhQvkXmZW"
FirebaseData firebaseData;
Servo myservo;
#define DHTPIN D8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int lampLivingRoom = D0;
int lampBedRoom = D1;
int lampKitchen = D2;
int lampToilet = D3;
int fan = D4;
int horn = D5;
int door = D6;
int gasSS = D7;

void setup() {
  pinMode(lampLivingRoom, OUTPUT);
  pinMode(lampBedRoom, OUTPUT);
  pinMode(lampKitchen, OUTPUT);
  pinMode(lampToilet, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(horn, OUTPUT);
  pinMode(door, OUTPUT);
  myservo.attach(door);
  Serial.begin(115200);
  Serial.println("Wifi connect");
  //ket noi
  WiFi.begin("QUANG", "123456789vu");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  dht.begin();

}

void loop() {
  Serial.println(analogRead(gasSS));
  if (analogRead(gasSS) > 1000) Firebase.setString(firebaseData, "Sensor/gas", "1");
  else Firebase.setString(firebaseData, "Sensor/gas", "0");
  Firebase.setFloat(firebaseData, "Sensor/temp", dht.readTemperature());
  if (Firebase.getString(firebaseData, "/Lamp/livingRoom")) {
    if ((firebaseData.stringData() == "ON")) {
      digitalWrite(lampLivingRoom, 1);
    } else digitalWrite(lampLivingRoom, 0);
  }

  if (Firebase.getString(firebaseData, "/Lamp/bedRoom")) {
    if ((firebaseData.stringData() == "ON")) {
      digitalWrite(lampBedRoom, 1);
    } else  digitalWrite(lampBedRoom, 0);
  }
  if (Firebase.getString(firebaseData, "/Lamp/kitchen")) {
    if ((firebaseData.stringData() == "ON")) {
      digitalWrite(lampKitchen, 1);
    } else  digitalWrite(lampKitchen, 0);
  }
  if (Firebase.getString(firebaseData, "/Lamp/toilet")) {
    if ((firebaseData.stringData() == "ON")) {
      digitalWrite(lampToilet, 1);
    } else  digitalWrite(lampToilet, 0);
  }
  if (Firebase.getString(firebaseData, "/Device/fan")) {
    if ((firebaseData.stringData() == "ON")) {
      digitalWrite(fan, 1);
    } else digitalWrite(fan, 0);
  }
  if (Firebase.getString(firebaseData, "/Sensor/gas")) {
    if ((firebaseData.stringData() == "1")) {
      digitalWrite(horn, 1);
    } else digitalWrite(horn, 0);
  }
  if (Firebase.getString(firebaseData, "/Device/horn")) {
    if ((firebaseData.stringData() == "ON")) {
      digitalWrite(horn, 1);
    } else digitalWrite(horn, 0);
  }
  if (Firebase.getString(firebaseData, "/Device/door")) {
    if ((firebaseData.stringData() == "ON")) {
      myservo.write(30);
    } else myservo.write(180);
  }

}
