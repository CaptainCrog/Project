// The MQTT callback function for commands and configuration updates
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
#include <NewPing.h>
#include <TinyGPS++.h>
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
float lattitude,longitude;
float crashLat, crashLng;
float duration, distance;
void setup() 
{

 gpsSerial.begin(9600);
 Serial.begin(9600);

 pinPeripheral(GPS_RXD_PIN, PIO_SERCOM); //Assign RX function to pin 0
 pinPeripheral(GPS_TXD_PIN, PIO_SERCOM); //Assign TX function to pin 1

 setupCloudIoT();
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
    crashLat = lattitude;
    crashLng = longitude;
    Serial.println(crashLat, 6);
    Serial.println(crashLng, 6);
    String payload = 
      String(",\"lattitude co-ordinates\":") + crashLat +
      String(",\"longitude co-ordinates\":") + crashLng +
      String(",\"car name\":") + carName + 
      String(",\"license plate\":") + licensePlate + 
      String(",\"car owner\":") + carOwner + 
      String(",\"owner's address\":") + ownerAddress + 
      String("}");
    publishTelemetry(payload);
    delay(10000);
  }
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
      lattitude = (gps.location.lat());
      longitude = (gps.location.lng());
      Serial.print ("lattitude: ");
      Serial.println (lattitude,6);
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
