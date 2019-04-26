#include <Arduino.h>
#include "hac-esp.h"

void setup() {
		if (DEBUG && !LCDDEBUG) Serial.begin(9600);
		setupLCD();
		setupMQTT();
		setupRFID();
		setupPZEM();
		setupSSR();
}

char cur_carduid[20] = {};

void loop(){
	client.loop();

	char carduid[20];
	if (activate == 0) {
		if (!client.connected()) {
			connect();
			connectedToRaspi = 0;
			//if (connectedToRaspi == 0) unconnectedToBroker();
		}
		if (!raspiConnected()) {
			connectionLossScreen();
		}
		else {
			welcomeScreen();
			if (getCardUID(carduid) && strcmp(carduid, cur_carduid) != 0) {
				strcpy(cur_carduid, carduid);
				printDebug(carduid);
				String topic = String(machine_id) + "/state/carduid";
				client.publish(topic, carduid, false, 2);
			}
		}

	}
	else if (activate == 1 ){
		digitalWrite(SSR_PIN, HIGH);
		currentMillis = millis();
		int elapseTime = (currentMillis - startMillis)/60000;
		int remainingTime = interval/60000 - elapseTime;
		if (currentMillis - startMillis < interval){
			float v = pzem.voltage(ip);
			float i = pzem.current(ip);
			float p = pzem.power(ip);
			float e = pzem.energy(ip) - startEnergy;
			float pf = (p/(v*i));
			char buffer4[4];
			char buffer5[4];
			String powerFactor = dtostrf(pf , 4, 0, buffer4);
			String energys = dtostrf(e , 4, 0, buffer5);
			// certifiedScreen(String(remainingTime), energys);
			pzemScreen(String(remainingTime), v, i, p, e);
			if(currentMillis - minuteMillis >= 60000){
				String topicEnergy = String(machine_id) + "/state/usage";
				client.publish(topicEnergy, energys, false, 2);
				minuteMillis = currentMillis;
			}
			if (remainingTime <= 3){
				lcdBlink = 1;
				if (getCardUID(carduid) && strcmp(carduid, cur_carduid) == 0){
					lcdBlink = 0;
					startMillis = currentMillis;
				}
			}
		}

		int stopButton = digitalRead(BUTTON_PIN);
		if (currentMillis - startMillis >= interval || stopButton == 1){
			float e = pzem.energy(ip) - startEnergy;
			char buffer4[4];
			String energys = dtostrf(e , 4, 0, buffer4);
			String topicEnergy = String(machine_id) + "/state/stop";
			client.publish(topicEnergy, energys, false, 2);
			digitalWrite(SSR_PIN, LOW);
			activate = 0;
			lcdBlink = 0;
			cur_carduid[0]='\0';
			endScreen();
			delay(2500);
		}
		/*
		if (!client.connected()) connectedToRaspi = 0;
		if (!connectedToRaspi){
			float e = pzem.energy(ip) - startEnergy;
			char buffer4[4];
			String energys = dtostrf(e , 4, 0, buffer4);
			String topicStop = String(machine_id) + "/state/stop";
			digitalWrite(SSR_PIN, LOW);
			while(!client.connected()) connect();
			client.publish(topicStop, energys);
			activate = 0;
			lcdBlink = 0 ;
			cur_carduid[0] = '\0';
			connectionLossScreen();
			delay(2500);
		}
		*/

		// if (!client.connected()) {
		// 	connect();
		// 	connectedToRaspi = 0;
		// }
		if (!raspiConnected()) {
			float e = pzem.energy(ip) - startEnergy;
			char buffer4[4];
			String energys = dtostrf(e , 4, 0, buffer4);
			String topicStop = String(machine_id) + "/state/stop";
			digitalWrite(SSR_PIN, LOW);
			while(!client.connected()) connect();
			while(!raspiConnected()) {
				printDebug("reconnecting");
				client.loop();
				delay(200);
			}
			client.publish(topicStop, energys, false, 2);
			activate = 0;
			lcdBlink = 0 ;
			cur_carduid[0] = '\0';
			connectionLossScreen();
			delay(2500);
		}

		if (lcdBlink == 1 && currentMillis - lcdMillis >= 1000){
			lcdBacklight = 255 - lcdBacklight;
			lcd.setBacklight(lcdBacklight);
			lcdMillis = currentMillis;
		}
	}
}
