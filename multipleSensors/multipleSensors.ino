
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 0
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

const int MotionSensorSignal = 5;  // Digital port for the motion sensor

int motionSensorValue = 0; 


int DA=A0;  // Pin for Analog Output - AO
int threshold = 100; //Change this
int lightSensorValue=0;


void setup() {

  Serial.begin(9600);
  pinMode(MotionSensorSignal, INPUT); // declare digital pin 2 as input, this is where you connect the S output from your sensor, this can be any digital pin

sensors.begin();

}

void loop() {

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
  Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? 
    // You can have more than one IC on the same bus. 
    // 0 refers to the first IC on the wire

//LUX

  lightSensorValue = analogRead(DA);  //Read the analog value
  Serial.print("Light is: ");  
  Serial.println(lightSensorValue);  //Print the analog value

  delay(5000);

}
