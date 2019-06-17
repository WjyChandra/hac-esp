#include <ESP8266WiFi.h>
#include <MQTT.h>

const char* ssid        = "E1000-Test";
const char* password    = "856427319";
const char* mqtt_server = "192.168.2.110";
const int   mqtt_port   = 1883;

const int mqtt_qos = 1;
const bool mqtt_retain = false;
const int mqtt_keepAlive = 60;
const bool mqtt_cleanSession = true; //changed on 15/05/2019 from true to false
const int mqtt_timeout = 1000;

WiFiClient net;
MQTTClient client;

void mqttPublish(String topic, String payload, int n=5) {
	for (int i = 0; i < n; i++)
		client.publish(topic, payload, mqtt_retain, mqtt_qos);
}

void connect(unsigned long timeout=3000) {
	unsigned long connectMillis = millis();

	while (WiFi.status() != WL_CONNECTED && (millis()-connectMillis < timeout)) {
		printDebug("Connecting to WiFi...");
		delay(1000);
	}
	while (!client.connect(machine_name) && (millis()-connectMillis < timeout)) {
		printDebug("Connecting to MQTT Broker...");
		delay(1000);
	}

	// String topic = String(machine_id) + "/command/#";
  	// client.subscribe(topic, mqtt_qos);

	String topic = String(machine_id) + "/command/action";
  	client.subscribe(topic, mqtt_qos);
	topic = String(machine_id) + "/command/connect";
  	client.subscribe(topic, mqtt_qos);
}

void connectRaspi() {
	String topics = String(machine_id) + "/state/connect";
	// client.publish(topics, "?", mqtt_retain, mqtt_qos);
	connect_ID++;
	String payload = String(connect_ID);
	mqttPublish(topics, payload);
	raspiResponse = 0;
	raspiMillis = millis();
	if (firstRaspiConnect) firstRaspiConnect = 0;
}

bool raspiConnected() {
	unsigned long curRaspiMillis = millis();
	if (firstRaspiConnect) {
		connectRaspi();
		return true;
	}
	if (!connectedToRaspi) {
		if (curRaspiMillis-raspiMillis > raspiInterval) connectRaspi();
		return false;
	}
	if (connectedToRaspi){
		if (curRaspiMillis-raspiMillis > raspiInterval) {
			if (raspiResponse == 0) {
				failToConnect++;
				if (failToConnect >= 10) {
					failToConnect = 0;
					connectedToRaspi = 0;
					return false;
				}
			}
			else {
				failToConnect = 0;
			}
			connectRaspi();
		}
		return true;
	}
	return false;
}

void messageReceived(String &topic, String &payload) {
	String debugMessage = "in " + topic + ":" + payload;
	printDebug(debugMessage);

	String topicRef = String(machine_id) + "/command/action";
	if (topicRef == topic){
		if (payload == "0") {
	    	activate = 0;
		}
		else if (payload == "1"){
			lcdPrint("Starting New Session");
			activate	= 1;
			startMillis = millis();
			minuteMillis = startMillis;
			startEnergy = pzem.energy(ip);
			raspiMillis = millis();
			raspiResponse = 1;
			connectedToRaspi = 1;
		}
		else if (payload == "2"){
			deniedScreen();
			delay(2500);
			welcomeScreen();
		}
		else if (payload == "3"){
			unknownCardScreen();
			delay (2500);
			welcomeScreen();
		}
	}
	topicRef = String(machine_id) + "/command/connect";
	if (topicRef == topic){
		raspiResponse = 1;
		raspiMillis = millis();
		connectedToRaspi = 1;
		failToConnect = 0;
		isBooting = 0;
	}
	raspiResponse = 1;
	raspiMillis = millis();
	connectedToRaspi = 1;
	failToConnect = 0;
}

void setupMQTT() {
	WiFi.begin(ssid, password);
	client.begin(mqtt_server, mqtt_port, net);
	client.setOptions(mqtt_keepAlive, mqtt_cleanSession, mqtt_timeout);
	client.onMessage(messageReceived);
	connect(5000);
	raspiConnected();
}
