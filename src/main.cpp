#include <Arduino.h>
#include "hac-esp.h"

void setup() {
		if (DEBUG && !LCDDEBUG) Serial.begin(9600);
		setupLCD();
		setupMQTT();
		setupRFID();
		setupPZEM();
		setupSSR();
		String topic = String(machine_id) + "/state/resetconnect";
		mqttPublish(topic,"", 1);
}

char cur_carduid[20] = {};

void loop(){
	client.loop();
	if (!client.connected()) connect();

	char carduid[20];

	if (activate == 1 ){
		/*
		// test code
		if (!responseTimePrinted) {
			responseTimePrinted = 1;
			lcdPrint(String(millis()-readCardMillis));
			delay(3000);
		}
		// test code
		*/

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
			char buffer[5];
			String powerFactor = dtostrf(pf , 4, 0, buffer);
			String energys = dtostrf(e , 4, 0, buffer);
			certifiedScreen(String(remainingTime), energys);

			if(currentMillis - minuteMillis >= 60000){
				String topicEnergy = String(machine_id) + "/state/usage";
				mqttPublish(topicEnergy, energys, 1);
				minuteMillis = currentMillis;
			}
			if (remainingTime <= 3){
				lcdBlink = 1;
				if (getCardUID(carduid) && strcmp(carduid, cur_carduid) == 0){
					lcdBlink = 0;
					lcdBacklight = 255;
					lcd.setBacklight(lcdBacklight);
					startMillis = currentMillis;
				}
			}
		}

		int stopButton = digitalRead(BUTTON_PIN);
		if ((currentMillis-startMillis >= interval) || stopButton==1){
			float e = pzem.energy(ip) - startEnergy;
			char buffer[5];
			String energys = dtostrf(e , 4, 0, buffer);
			String topicStop = String(machine_id) + "/state/stop";
			mqttPublish(topicStop, energys);
			digitalWrite(SSR_PIN, LOW);
			activate = 0;
			if (lcdBlink == 1) {
				lcdBlink = 0;
				lcdBacklight = 255;
				lcd.setBacklight(lcdBacklight);
			}
			memset(cur_carduid, 0, sizeof cur_carduid);
			endScreen();
			delay(2500);
		}

		if (!raspiConnected()) {
			float e = pzem.energy(ip) - startEnergy;
			char buffer[5];
			String energys = dtostrf(e , 4, 0, buffer);
			String topicStop = String(machine_id) + "/state/stop";
			digitalWrite(SSR_PIN, LOW);

			while(!client.connected()) {
				printDebug("reconnecting broker");
				connect();
			}
			while(!raspiConnected()) {
				printDebug("reconnecting raspi");
				delay(400);
				if (!client.connected()) connect();
				client.loop();
			}
			mqttPublish(topicStop, energys);
			activate = 0;
			if (lcdBlink == 1) {
				lcdBlink = 0;
				lcdBacklight = 255;
				lcd.setBacklight(lcdBacklight);
			}
			memset(cur_carduid, 0, sizeof cur_carduid);
			connectionLossScreen();
			delay(2500);
		}

		if (lcdBlink==1 && (currentMillis-lcdMillis >= 500)){
			lcdBacklight = 255 - lcdBacklight;
			lcd.setBacklight(lcdBacklight);
			lcdMillis = currentMillis;
		}
	}
	else if (activate == 0) {
		digitalWrite(SSR_PIN, LOW);
		if (lcdBlink == 1) {
			lcdBlink = 0;
			lcdBacklight = 255;
			lcd.setBacklight(lcdBacklight);
		}
		if (cur_carduid[0] != '\0')
			memset(cur_carduid, 0, sizeof cur_carduid);

		if (!raspiConnected()) {
			if (isBooting) bootingScreen();
			else connectionLossScreen();
		}
		else {
			/*
			// test code
			if (!bootTimePrinted) {
				lcdPrint(String(millis()));
				delay(3000);
				bootTimePrinted = 1;
			}
			// test code
			*/

			welcomeScreen();
			if (getCardUID(carduid) && strcmp(carduid, cur_carduid) != 0) {
				/*
				// test code
				readCardMillis = millis();
				responseTimePrinted = 0;
				// test code
				*/

				strcpy(cur_carduid, carduid);
				printDebug(carduid);
				String topic = String(machine_id) + "/state/carduid";
				mqttPublish(topic, carduid, 1);
				delay(100);
			}
		}

	}

}
