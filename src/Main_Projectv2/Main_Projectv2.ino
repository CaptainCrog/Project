// The MQTT callback function for commands and configuration updates
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
#include <NewPing.h>
#include <TinyGPS++.h>
#include <avr/dtostrf.h>
#include <LiquidCrystal.h>
#include "wiring_private.h"
#include "universal-mqtt.h"

#define TRIG_PIN 7 //Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 6 //Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 //Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define GPS_TXD_PIN 1
#define GPS_RXD_PIN 0

NewPing sr04(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
Uart gpsSerial (&sercom3, GPS_RXD_PIN, GPS_TXD_PIN, SERCOM_RX_PAD_1, UART_TX_PAD_0); // Create the new UART instance assigning it to pin 0 and 1
TinyGPSPlus gps;

//Car attributes
String carName = "IoT Car";
String licensePlate = "10T C4R";
String carOwner = "Craig Penning";
String ownerAddress = "29 Made-up Road";

//global variables
char Lat[10];
char Long[10];
float latitude,longitude;
float crashLat, crashLng;
float duration, distance;
String nl = ("/n");
void setup() 
{

 gpsSerial.begin(9600);
 Serial.begin(9600);
 lcd.begin(16,2);

 pinPeripheral(GPS_RXD_PIN, PIO_SERCOM); //Assign RX function to pin 0
 pinPeripheral(GPS_TXD_PIN, PIO_SERCOM); //Assign TX function to pin 1
 lcd.print("WiFi Connected?");
 setupCloudIoT();
 lcd.setCursor(0,1);
 lcd.print("Yes");
 delay(5000);
 lcd.clear();
}

void loop()
{
  mqttClient->loop();
  delay(10);
  if (!mqttClient->connected())
  {
    connect();
  }
  GetDist();
  GetGPS();
  if (distance <= 5)
  {
    Serial.println("CRASH");
    lcd.print("CRASH");
    delay(1000);
    lcd.clear();
    crashLat = latitude;
    crashLng = longitude;
    //Serial.println(crashLat, 6);
    //Serial.println(crashLng, 6);
    dtostrf(crashLat,6,6,Lat);
    dtostrf(crashLng,6,6,Long);
    Serial.println(Lat);
    Serial.println(Long);
    lcd.setCursor(0,0);
    lcd.print(Lat);
    lcd.setCursor(0,1);
    lcd.print(Long);
    String payload = 
      String("{");
      String(",\"latitude co-ordinates\":") + Lat + nl +
      String(",\"longitude co-ordinates\":") + Long + nl +
      String(",\"car name\":") + carName + 
      String(",\"license plate\":") + licensePlate + nl +
      String(",\"car owner\":") + carOwner + 
      String(",\"owner's address\":") + ownerAddress + nl +
      String("}") + nl + nl;
    publishTelemetry(payload);
    delay(10000);
  }
  lcd.clear();
  delay(50);
}

void GetGPS()
{
  //Serial.println("In GetGPS Function");
  while (gpsSerial.available())
  {
    //Serial.println("In GetGPS while loop");
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

void GetDist()
{
   distance=sr04.ping_cm();
   if (distance >= MAX_DISTANCE)
   {
    distance = 500;
   }
   Serial.print(distance);
   Serial.println("cm");
}
void SERCOM3_Handler()
{
  gpsSerial.IrqHandler();
}
