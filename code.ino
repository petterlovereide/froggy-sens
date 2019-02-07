#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "MY_FIREBASE_HOST"
#define FIREBASE_AUTH "MY_FIREBASE_AUTH"
#define WIFI_SSID "MY_SSID"
#define WIFI_PASSWORD "MY_PASSWORD"
#define LED 2

void connectToWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

void connectToFirebase() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (Firebase.failed()){
    Serial.print("Firebase init failed: ");
    Serial.println(Firebase.error());
  }
  if (Firebase.success()){
    Serial.println("Firebase init success!");
  }
}

void setup() {
  Serial.begin(9600);
  connectToWifi();
  connectToFirebase();
}

void loop() {
  Firebase.setInt("myValue", 123);
  int myValue = Firebase.getInt("myValue");
  Serial.print("myValue: ");
  Serial.println(myValue);

  // Print error if something failed
  if (Firebase.failed()) {
    Serial.print("Something failed: ");
    Serial.println(Firebase.error());
  }

  delay(10000);
}
