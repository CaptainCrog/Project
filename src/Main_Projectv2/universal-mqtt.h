#include <WiFiNINA.h>
#include <WiFiSSLClient.h>
#include <MQTT.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#include "ciotc_config.h" // Update this file with your configuration

///////////////////////////////
// Cloud IoT configuration that you don't need to change
Client *netClient;
CloudIoTCoreDevice *device;
CloudIoTCoreMqtt *mqtt;
MQTTClient *mqttClient;
unsigned long iss = 0;
String jwt;

///////////////////////////////
// Helpers specific to this board
///////////////////////////////
String getDefaultSensor() {
	return  "Wifi: " + String(WiFi.RSSI()) + "db";
}

String getJwt() {
	// Disable software watchdog as these operations can take a while.
	Serial.println("Refreshing JWT");
	iss = WiFi.getTime();
	jwt = device->createJWT(iss, jwt_exp_secs);
	return jwt;
}

void setupWifi() {
	Serial.println("Starting wifi");

	WiFi.begin(ssid, password);
	Serial.println("Connecting to WiFi");
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
	}

	Serial.println("Waiting on time sync...");
	while (WiFi.getTime() < 1510644967) {
		delay(10);
	}
}

void connectWifi() {
	Serial.print("checking wifi...");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(1000);
	}
}

///////////////////////////////
// Orchestrates various methods from preceeding code.
///////////////////////////////
void connect() {
	connectWifi();
	mqtt->mqttConnect();
}

void publishTelemetry(String data) {
	mqtt->publishTelemetry(data);
}

void publishTelemetry(String subfolder, String data) {
	mqtt->publishTelemetry(subfolder, data);
}

void setupCloudIoT() {
	device = new CloudIoTCoreDevice(
		project_id, region, registry_id, device_id,
		private_key_str);

	setupWifi();
	netClient = new WiFiSSLClient();

	mqttClient = new MQTTClient(512);
	mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
	mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);
	mqtt->startMQTT();
}
