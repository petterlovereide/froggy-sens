#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define FIREBASE_HOST "flying-hacker-frogs.firebaseio.com"
#define FIREBASE_AUTH "ykor3oRgZ8UYirKJf827ZHZkXnqEa8xwxiMvNv0j"
#define WIFI_SSID "HACKERMAN"
#define WIFI_PASSWORD "RaspBerryPi19"
#define LED 2
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

const int MotionSensorSignal = 5;  // Digital port for the motion sensor

int motionSensorValue = 0;

float tempSensorValue = 0.0;

int DA=A0;  // Pin for Analog Output - AO
int threshold = 100; //Change this
float lightSensorValue = 0.0;


///////////////////////////////////////////

void sensorLoop() {

 //MOTION

 motionSensorValue = digitalRead(MotionSensorSignal);

 Serial.print("This is the value from the motion sensor: ");
 Serial.println(motionSensorValue);


//TEMP

 // call sensors.requestTemperatures() to issue a global temperature
 // request to all devices on the bus
 Serial.print(" Requesting temperatures...");
 sensors.requestTemperatures(); // Send the command to get temperatures
 Serial.println("DONE");

 Serial.print("Temperature is: ");
 tempSensorValue = sensors.getTempCByIndex(0);
 Serial.println(tempSensorValue); // Why "byIndex"?
   // You can have more than one IC on the same bus.
   // 0 refers to the first IC on the wire

//LUX

 lightSensorValue = analogRead(DA);  //Read the analog value
 Serial.print("Light is: ");
 Serial.println(lightSensorValue);  //Print the analog value

}

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

///////////////////////////////////////////

void setup() {
 Serial.begin(9600);
 connectToWifi();
 connectToFirebase();

 pinMode(MotionSensorSignal, INPUT); // declare digital pin 2 as input, this is where you connect the S output from your sensor, this can be any digital pin
 sensors.begin();
}

void loop() {

 /*
 Firebase.setInt("myValue", 123);
 int myValue = Firebase.getInt("myValue");
 Serial.print("myValue: ");
 Serial.println(myValue);
 */

 // Print error if something failed
 if (Firebase.failed()) {
   Serial.print("Something failed: ");
   Serial.println(Firebase.error());
 }

 sensorLoop();

 if(tempSensorValue < -50) {
   return;
   }
/*
 Firebase.setFloat("temp", tempSensorValue);
 Firebase.setBool("motion", motionSensorValue);
 Firebase.setFloat("lux", lightSensorValue);
*/
 DynamicJsonBuffer jsonBuffer;

 // Push to Firebase
 JsonObject& temperatureObject = jsonBuffer.createObject();
 JsonObject& tempTime = temperatureObject.createNestedObject("timestamp");
 temperatureObject["light"] = lightSensorValue;
 temperatureObject["motion"] = motionSensorValue;
 temperatureObject["temperature"] = tempSensorValue;
 tempTime[".sv"] = "timestamp";

 Firebase.push("sensors/300", temperatureObject);

 delay(10000);
}
