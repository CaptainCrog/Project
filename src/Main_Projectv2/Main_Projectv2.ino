// The MQTT callback function for commands and configuration updates
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
// Libraries used for the project (NOTE: Libraries dedicated to connecting the device to the cloud/wifi can be found in the universal-mqtt.h file
#include <NewPing.h>                        // Ultrasonic Sencor
#include <TinyGPS++.h>                      // GPS Tracker
#include <avr/dtostrf.h>                    // Allows for conversion of floats to char arrays
#include <LiquidCrystal.h>                  // LCD Screen
#include "wiring_private.h"
#include "universal-mqtt.h"                 // IoT Core/WiFi file

// PIN DEFINITIONS
#define TRIG_PIN 7                          // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 6                          // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500                    // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define GPS_TXD_PIN 1                       // GPS Transmitting pin
#define GPS_RXD_PIN 0                       // GPS Receiving pin

// OBJECT INSTANTIATIONS
NewPing sr04(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
Uart gpsSerial (&sercom3, GPS_RXD_PIN, GPS_TXD_PIN, SERCOM_RX_PAD_1, UART_TX_PAD_0); // Create the new UART instance assigning it to pin 0 and 1, UART is a manually created serial port
TinyGPSPlus gps;
LiquidCrystal lcd(8,9,2,3,4,5);

// CAR ATTRIBUTES (Made up attributes for the sake of demoing the idea
String car_model = "IoT Car";                // Model of the car (real life examples could be Nissan Micra, Ford Fiesta, etc.)
String license_plate = "10T C4R";            // License plate of the car (contested idea as this could be stored by the DVLA rather than the car itself)
String car_owner = "Craig Penning";          // Owner details
String owner_address = "29 Made-up Road";    // Owner's address
String car_VIN = "2HGFA1F54BH557624";        // VIN number that is assigned to every car (generated from http://www.randomvin.com/)

//global variables
char crash_lat[10];                          // char array that stores the lattitude data
char crash_lng[10];                          // char array that stores the longitude data
float latitude,longitude;                   // floats which store the changing GPS data
float distance;                             // float which stores the distance between objects and the ultrasonic sensor (in cm)
String nl = ("/n");                         // Adds a newline

// Setup function which runs once
void setup() 
{
  // Create serial communication for the GPS and US sensor
  gpsSerial.begin(9600);
  Serial.begin(9600);
  lcd.begin(16,2);                          // Set up the LCD's number of columns and rows
  
  pinPeripheral(GPS_RXD_PIN, PIO_SERCOM);   // Assign RX function to pin 0
  pinPeripheral(GPS_TXD_PIN, PIO_SERCOM);   // Assign TX function to pin 1
  lcd.print("WiFi Connected?");             // Print out to LCD screen before the WiFi/Cloud is setup
  setupCloudIoT();                          // Call function
  lcd.setCursor(0,1);                       // Change LCD output to second row
  lcd.print("Yes");                         // Print out to LCD screen if connection is successful
  delay(5000);                              // Wait 5 Seconds
  lcd.clear();                              // Clear text on LCD screen
}

// Main code goes here and runs repeatedly
void loop()
{
  // Ensures MQTT client is connected
  mqttClient->loop();
  delay(10);
  if (!mqttClient->connected())
  {
    connect();
  }
  // Get Distance and GPS data
  getDist();
  getGPS();
  // If the distance between an object and the sensore is less than the set distance, a crash is recorded and a payload of data is published to the IoT Core on Google Cloud
  if (distance <= 5)
  {
    Serial.println("CRASH");
    lcd.print("CRASH");
    delay(1000);
    lcd.clear();
    dtostrf(latitude,6,6,crash_lat);
    dtostrf(longitude,6,6,crash_lng);
    Serial.println(crash_lat);
    Serial.println(crash_lng);
    lcd.setCursor(0,0);
    lcd.print(crash_lat);
    lcd.setCursor(0,1);
    lcd.print(crash_lng);
    String payload = 
      String("{");
      String(",\"latitude co-ordinates\":") + crash_lat + nl +
      String(",\"longitude co-ordinates\":") + crash_lng + nl +
      String(",\"car model\":") + car_model + 
      String(",\"license plate\":") + license_plate + nl +
      String(",\"car owner\":") + car_owner + 
      String(",\"owner's address\":") + owner_address + nl +
      String("}") + nl + nl;
    publishTelemetry(payload);
    delay(10000);
  }
  lcd.clear();
  delay(50);
}

// Function that gets the GPS co-ordinates
void getGPS()
{
  //Serial.println("In GetGPS Function");
  while (gpsSerial.available())
  {
    //Serial.println("In GetGPS while loop");
    // Reads the GPS data into two float variables and outputs them to the Serial Monitor and the LCD Screen
    float data = gpsSerial.read();
    if (gps.encode(data))
    {
      latitude = (gps.location.lat());
      longitude = (gps.location.lng());
      Serial.print ("latitude: ");
      Serial.println (latitude,6);
      Serial.print ("longitude: ");
      Serial.println (longitude,6);
      Serial.println("");
    }
  }
}

// Function that retrieves the distance data from the Ultrasonic Sensor
void getDist()
{
   // Ping the sensor and store the distance,if distance is over the maximum then it sets the distance as a flat value (without this it stores 0 which would imply a crash has occured). 
   distance=sr04.ping_cm();
   if (distance >= MAX_DISTANCE)
   {
    distance = 500;
   }
   // Print data to the Serial Monitor
   Serial.print(distance);
   Serial.println("cm");
}

//Function for handling the serial communication port that has been manually assigned
void SERCOM3_Handler()
{
  gpsSerial.IrqHandler();
}
