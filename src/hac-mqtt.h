#include <ESP8266WiFi.h>
#include <MQTT.h>

// const char* ssid        = "Linksys25927-guest";
// const char* mqtt_server = "192.168.3.156";

// const char* ssid        = "HAC-AP";
// const char* password    = "123456789";
// const char* mqtt_server = "192.168.137.130";

// const char* ssid        = "Android AP";
// const char* password    = "abcdefgh";
// const char* mqtt_server = "192.168.43.215";

const char* ssid        = "E1000-Test";
const char* password    = "856427319";
const char* mqtt_server = "192.168.2.110";

// const char* ssid        = "MasterBulb";
// const char* password    = "celab123";

const int mqtt_port = 1883;
const int mqtt_qos = 2;
const bool retained = false;

WiFiClient net;
MQTTClient client;

void connect() {
	printDebug("Checking WiFi...");
	while (WiFi.status() != WL_CONNECTED) {
		printDebug("Connecting to WiFi");
		delay(100);
	}
	printDebug("WiFi connected!");
	while (!client.connect(machine_name)) {
		printDebug("Connecting to MQTT Broker");
		delay(100);
	}
	printDebug("MQTT connected!");

	String topic = String(machine_id) + "/command/#";
  	client.subscribe(topic, mqtt_qos);
	// printDebug("sub to " + topic);
}

void connectRaspi() {
	// printDebug("Waiting for Raspi...");
	String topics = String(machine_id) + "/state/connect";
	String msg = String(failToConnect);
	client.publish(topics, msg, retained, mqtt_qos);
	raspiResponse = 0;
	raspiMillis = millis();
	if (firstRaspiConnect) firstRaspiConnect = 0;
}

bool raspiConnected() {
	unsigned long curRaspiMillis = millis();
	if (firstRaspiConnect) {
		connectRaspi();
		return false;
	}
	else if (!connectedToRaspi) {
		if (curRaspiMillis-raspiMillis > raspiInterval) connectRaspi();
		return false;
	}
	else if (connectedToRaspi) {
		if (curRaspiMillis-raspiMillis > raspiInterval) {
			if (raspiResponse == 0) {
				failToConnect++;
				if (failToConnect >= 5) {
					connectedToRaspi = 0;
					failToConnect = 0;
					return false;
				}
			}
			else if (raspiResponse == 1) {
				failToConnect = 0;
				raspiMillis = millis();
			}
			connectRaspi();
		}
		return true;
	}
	return false;
}

void messageReceived(String &topic, String &payload) {
	// String debugMessage = "<" + topic + ":" + payload;
	// printDebug(debugMessage);

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
			failToConnect = 0;
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
		raspiMillis = millis();
		raspiResponse = 1;
		connectedToRaspi = 1;
		failToConnect = 0;
	}
}

void setupMQTT() {
	WiFi.begin(ssid, password);
	// WiFi.begin(ssid);
	client.begin(mqtt_server, mqtt_port, net);
	client.onMessage(messageReceived);
	client.setOptions(60, false, 1000);
	connect();
}
