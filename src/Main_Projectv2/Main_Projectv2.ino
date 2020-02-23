// The MQTT callback function for commands and configuration updates
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
// Libraries used for the project (NOTE: Libraries dedicated to connecting the device to the cloud/wifi can be found in the universal-mqtt.h file
#include <NewPing.h>                        // Ultrasonic Sencor
#include <TinyGPS++.h>                      // GPS Tracker
#include <LiquidCrystal.h>                  // LCD Screen
#include "wiring_private.h"
#include "universal-mqtt.h"                 // IoT Core/WiFi file
#include <twilio.hpp>


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
Twilio *twilio;

// Car variables (Made up attributes for the sake of demoing the idea
String car_model = "IoT Car";                // Model of the car (real life examples could be Nissan Micra, Ford Fiesta, etc.)
String license_plate = "10T C4R";            // License plate of the car (contested idea as this could be stored by the DVLA rather than the car itself)
String car_owner = "Craig Penning";          // Owner details
String owner_address = "29 Made-up Road";    // Owner's address
String car_VIN = "2HGFA1F54BH557624";        // VIN number that is assigned to every car (generated from http://www.randomvin.com/)

// Component variables
char crash_lat[10];                          // char array that stores the lattitude data
char crash_lng[10];                          // char array that stores the longitude data
float latitude,longitude;                   // floats which store the changing GPS data
float distance;                             // float which stores the distance between objects and the ultrasonic sensor (in cm)
String nl = ("/n");                         // Adds a newline

// Twilio variables
static const char *account_sid = "";
static const char *auth_token = "";
// Phone number from Twilio (put countrycode before)
static const char *from_number = "+";
// Phone number to recieve message (put countrycode before)
static const char *to_number = "+";

// Setup function which runs once
void setup() 
{
  // Create serial communication for the GPS and US sensor
  gpsSerial.begin(9600);
  Serial.begin(9600);
  lcd.begin(16,2);                          // Set up the LCD's number of columns and rows
  twilio = new Twilio(account_sid, auth_token);
  
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
    String message = String(car_model + " has crashed at Latitude: " + crash_lat + " and Longitude: " + crash_lng);
    Serial.println(message);
    String response;
    twilio->send_message(to_number, from_number, message, response);
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
// Function converts float to string as a char array
char *dtostrf (float val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}
// Library code directly injected into the main file, sourced from https://github.com/ademuri/twilio-esp32-client
/*
   Send a SMS or MMS with the Twilio REST API

   Inputs:
    - to_number : Number to send the message to
    - from_number : Number to send the message from
    - message_body : Text to send in the message (max 1600 characters)
    - picture_url : (Optional) URL to an image

   Outputs:
    - response : Connection messages and Twilio responses returned to caller
    - bool (method) : Whether the message send was successful
*/
bool Twilio::send_message(const String& to_number, const String& from_number, const String& message_body, String& response, const String& picture_url)
{
  // Check the body is less than 1600 characters in length.  see:
  // https://support.twilio.com/hc/en-us/articles/223181508-Does-Twilio-support-concatenated-SMS-messages-or-messages-over-160-characters-
  // Note this is only checking ASCII length, not UCS-2 encoding; your
  // application may need to enhance this.
  if (message_body.length() > 1600) {
    response += "Message body must be 1600 or fewer characters.";
    response += "  You are attempting to send ";
    response += message_body.length();
    response += ".\r\n";
    return false;
  }

  // URL encode our message body & picture URL to escape special chars
  // such as '&' and '='
  String encoded_body = urlencode(message_body);

  // Use WiFiSSLClient class to create TLS 1.2 connection
  const char* host = "api.twilio.com";
  const int   httpsPort = 443;

  // Use WiFiSSLClient class to create TLS connection
  Serial.print("connecting to ");
  Serial.println(host);

  //Serial.printf("Using cert '%s'\n", ca_crt);

  // Connect to Twilio's REST API
  response += ("Connecting to host ");
  response += host;
  response += "\r\n";
  if (!netClient->connect(host, httpsPort)) {
    response += ("Connection failed!\r\n");
    return false;
  }

  // Attempt to send an SMS or MMS, depending on picture URL
  String post_data = "To=" + urlencode(to_number) + "&From=" + urlencode(from_number) + \
                     "&Body=" + encoded_body;
  if (picture_url.length() > 0) {
    String encoded_image = urlencode(picture_url);
    post_data += "&MediaUrl=" + encoded_image;
  }

  // Construct headers and post body manually
  String auth_header = _get_auth_header(account_sid, auth_token);
  String http_request = "POST /2010-04-01/Accounts/" +
                        String(account_sid) + "/Messages HTTP/1.1\r\n" +
                        auth_header + "\r\n" + "Host: " + host + "\r\n" +
                        "Cache-control: no-cache\r\n" +
                        "User-Agent: ESP8266 Twilio Example\r\n" +
                        "Content-Type: " +
                        "application/x-www-form-urlencoded\r\n" +
                        "Content-Length: " + post_data.length() + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n" + post_data + "\r\n";

  response += ("Sending http POST: \r\n" + http_request);
  netClient->println(http_request);

  // Read the response into the 'response' string
  response += ("request sent");
  bool prev_was_empty = false;
  while (netClient->connected()) {
    String line = netClient->readStringUntil('\n');
    // Two empty lines in a row and we'll assume it's done - otherwise this never terminates (?)
    if (line.length() == 0) {
      if (prev_was_empty) {
        break;
      }
      prev_was_empty = true;
    }
    response += (line);
    response += ("\r\n");
  }
  return true;
}

/* Private function to create a Basic Auth field and parameter */
String Twilio::_get_auth_header(const String& user, const String& password) {
  size_t toencodeLen = user.length() + password.length() + 2;
  char toencode[toencodeLen];
  memset(toencode, 0, toencodeLen);
  snprintf(
    toencode,
    toencodeLen,
    "%s:%s",
    user.c_str(),
    password.c_str()
  );

  rbase64.encode((uint8_t*)toencode, toencodeLen - 1);
  String encoded = rbase64.result();
  String encoded_string = String(encoded);

  // Strip newlines (after every 72 characters in spec)
  int i = 0;
  while (i < encoded_string.length()) {
    i = encoded_string.indexOf('\n', i);
    if (i == -1) {
      break;
    }
    encoded_string.remove(i, 1);
  }
  return "Authorization: Basic " + encoded_string;
}


//Function for handling the serial communication port that has been manually assigned
void SERCOM3_Handler()
{
  gpsSerial.IrqHandler();
}
