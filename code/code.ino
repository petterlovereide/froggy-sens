#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define FIREBASE_HOST "flying-hacker-frogs.firebaseio.com"
#define FIREBASE_AUTH "ykor3oRgZ8UYirKJf827ZHZkXnqEa8xwxiMvNv0j"
#define WIFI_SSID "HACKERMAN"
#define WIFI_PASSWORD "RaspBerryPi19"

#define BUILDING "Voksenåsen Hotell"
#define SENSORID "100"

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 0

///////////////////////////////////////////

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

bool isIdentifying = false;

String macAdr = "";

const int MotionSensorSignal = 5;  // Digital port for the motion sensor
int motionSensorValue = 0;

float tempSensorValue = 0.0;

const int LightSensorSignal = A0;  // Pin for Analog Output - AO
float lightSensorValue = 0.0;

// Led Lights
int ledRedPin = 4;
int ledGreenPin = 2;

///////////////////////////////////////////

void sensorLoop() {
//MOTION
 motionSensorValue = digitalRead(MotionSensorSignal);
 Serial.print("This is the value from the motion sensor: ");
 Serial.println(motionSensorValue);

//TEMP
 // call sensors.requestTemperatures() to issue a global temperature
 // request to all devices on the bus
 sensors.requestTemperatures(); // Send the command to get temperatures

 Serial.print("Temperature is: ");
 tempSensorValue = sensors.getTempCByIndex(0);
 Serial.println(tempSensorValue);
 // You can have more than one IC on the same bus.
 // 0 refers to the first IC on the wire

//LIGHT
 lightSensorValue = analogRead(LightSensorSignal);  //Read the analog value
 Serial.print("Light is: ");
 Serial.println(lightSensorValue);  //Print the analog value
}

void blinkLED(int pin){
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
}

void connectToWifi() {
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 Serial.print("Connecting to WIFI");
 while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   blinkLED(ledRedPin);
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
   digitalWrite(ledRedPin, HIGH);
 }
 else if (Firebase.success()){
   Serial.println("Firebase init success!");
 }
}

void setupLEDs() {
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
}

void getIsIdentifying() {

  isIdentifying = Firebase.getBool("sensors/" + macAdr + "/isIdentifying");

  while(Firebase.getBool("sensors/" + macAdr + "/isIdentifying")){
    blinkLED(ledGreenPin);
    delay(500);
    }
  
}

void createSensorEntry() {

 Firebase.setBool("sensors/" + macAdr + "/isIdentifying", isIdentifying);
 Firebase.setString("sensors/" + macAdr + "/name", macAdr);

if(Firebase.success()){
  Serial.println("Firebase push success!");
  blinkLED(ledGreenPin);
 }
 else if(Firebase.failed()){
  Serial.println("Firebase push failed!");
  blinkLED(ledRedPin);
 }
  
}

///////////////////////////////////////////

void setup() {
 Serial.begin(9600);
 setupLEDs();
 connectToWifi();
 connectToFirebase();

 macAdr = WiFi.macAddress();

createSensorEntry();

 pinMode(MotionSensorSignal, INPUT); // declare digital pin 2 as input, this is where you connect the S output from your sensor, this can be any digital pin
 sensors.begin();
}

void loop() {
 getIsIdentifying();
  
 sensorLoop();

 if(tempSensorValue < -50) {
   return;
 }

 DynamicJsonBuffer jsonBuffer;

 // Push to Firebase
 JsonObject& temperatureObject = jsonBuffer.createObject();
 JsonObject& tempTime = temperatureObject.createNestedObject("timestamp");
 temperatureObject["light"] = lightSensorValue;
 temperatureObject["motion"] = motionSensorValue;
 temperatureObject["temperature"] = tempSensorValue;
 tempTime[".sv"] = "timestamp";

 Firebase.push("measurements/" + macAdr, temperatureObject);
 if(Firebase.success()){
  Serial.println("Firebase push success!");
  blinkLED(ledGreenPin);
 }
 else if(Firebase.failed()){
  Serial.println("Firebase push failed!");
  blinkLED(ledRedPin);
 }

 delay(9500);
}
