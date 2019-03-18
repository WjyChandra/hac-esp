#include"PZEM004T.h"
/*
   An example on how to use ESP8266 hardware serial with PZEM004T
   Note: ESP8266 UART0 pins GPIO1 (TX) and GPIO3 (RX) are usually connected
   to an onboard USB2serial converter, it can affect PZEM communication.
   It's better not to share USB2serial pins for PZEM communication
   Default UART0 pins could be swapped to gpio13(rx),gpio15(tx)
   Note: gpio15 pin must be pulled lOW on esp start, otherwise it won't boot from eeprom.
   PZEM004T always pulls it's RX pin HIGH via optocoupler's led, so make sure to workaround it anyhow while esp boots up
*/


     								// Use hwserial UART0 at pins GPIO1 (TX) and GPIO3 (RX)
PZEM004T pzem(3, 1);           		// Attach PZEM to hwserial
IPAddress ip(192,168,1,1);



void setupPZEM() {
      pzem.setAddress(ip);
      lcdPrint("Connecting To PZEM");
      delay(1000);
   }


void powerDisplay(){
	  float v = pzem.voltage(ip);
    if (v < 0.0) v = 0.0;
    Serial.print(v);Serial.print("V; ");
    char buffer1[4];
    String volt = dtostrf(v, 3 , 1, buffer1);
    lcd.setCursor(0, 0); lcd.print("V:"+volt);

    float i = pzem.current(ip);
    if(i >= 0.0){ Serial.print(i);Serial.print("A; "); }
    char buffer2[4];
    String currents = dtostrf(i, 2, 2, buffer2);
    lcd.setCursor(8, 0); lcd.print("A:"+currents);

    float p = pzem.power(ip);
    if(p >= 0.0){ Serial.print(p);Serial.print("W; "); }
    char buffer3[4];
    String powers = dtostrf(p, 4, 0, buffer3);
    lcd.setCursor(0, 1); lcd.print("W:"+powers);

    float e = pzem.energy(ip);
    if(e >= 0.0){ Serial.print(e);Serial.print("Wh; "); }
    char buffer4[4];
    String energys = dtostrf(e , 4, 0, buffer4);
    lcd.setCursor(8, 1); lcd.print("Wh:"+energys);

    Serial.println();

  	lcd.clear();

}
