#include <NewPing.h>
#include <TinyGPS++.h>
#include "wiring_private.h"
#include <jwt.h>
#include <MQTT.h>
#include <MQTTClient.h>

#define TRIG_PIN 7
#define ECHO_PIN 6
#define MAX_DISTANCE 100
#define GPS_TXD_PIN 1
#define GPS_RXD_PIN 0

NewPing sr04(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
Uart gpsSerial (&sercom3, GPS_RXD_PIN, GPS_TXD_PIN, SERCOM_RX_PAD_1, UART_TX_PAD_0); // Create the new UART instance assigning it to pin 0 and 1
//SoftwareSerial gpsSerial(GPS_TXD_PIN,GPS_RXD_PIN);
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
  if (!mqttclient->connected())
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
      String("{\"timestamp\":") + time(nullptr) +
      String(",\"lattitude co-ordinates\":") + crashLat +
      String(",\"longitude co-ordinates\":") + crashLng +
      String(",\"car name\":") + carName + 
      String(",\"license plate\":") + licensePlate + 
      String(",\"car owner\":") + carOwner + 
      String(",\"owner's address\":") + ownerAddress + 
      String("}");
    publishTelemetry(payload);
  }
  delay(50);
}

void GetGPS()
{
  while (gpsSerial.available())
  {
    float data = gpsSerial.read();
    if (gps.encode(data))
    {
      lattitude = (gps.location.lat());
      longitude = (gps.location.lng());
      //Serial.print ("lattitude: ");
      //Serial.println (lattitude,6);
      //Serial.print ("longitude: ");
      //Serial.println (longitude,6);
      //Serial.println("");
    }
  }
}

void GetDist()
{
   distance=sr04.ping_cm();
   Serial.print(distance);
   Serial.println("cm");
}
void SERCOM3_Handler()
{
  gpsSerial.IrqHandler();
}
