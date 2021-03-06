#include <SPI.h>
#include <Wire.h>

const int machine_id = 1;
const char* machine_name = "machine1";
const bool DEBUG = false;
const bool LCDDEBUG = false;
bool activate = 0; // value from cloud to ack SSR pin D8
unsigned long currentMillis, startMillis, minuteMillis, interval = 300000, previousMillis;
unsigned long lcdMillis;
unsigned long raspiMillis = 0, raspiInterval = 10000;
bool firstRaspiConnect = 1;
bool lcdBlink = 0;
int lcdBacklight = 255;
float startEnergy;
bool connectedToRaspi = 0, raspiResponse = 0;
int failToConnect = 0;
int connect_ID = 0;
bool isBooting = 1;
bool activate_init = 0;
/*
// test code
bool bootTimePrinted = 0, responseTimePrinted = 1;
unsigned long readCardMillis = 0;
*/


#define MFRC522_SS_PIN  D4
#define MFRC522_RST_PIN D3
#define SSR_PIN         D8
#define BUTTON_PIN      D0

void printDebug(String debugMessage);

#include "hac-lcd.h"
#include "hac-pzem004t.h"
#include "hac-mqtt.h"
#include "hac-rfid.h"


void printDebug(String debugMessage) {
    if (!DEBUG) return;
    if (LCDDEBUG) lcdPrint(debugMessage);
    else Serial.println(debugMessage);
}
void setupSSR(){
    // pinMode(BUTTON_PIN, INPUT);
    pinMode(SSR_PIN, OUTPUT);
}
