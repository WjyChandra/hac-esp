#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

void setupLCD() {
    Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();

    if (DEBUG && !LCDDEBUG) {
        Serial.print("Error: "); Serial.println(error);
        if (error == 0) Serial.println(": LCD found.");
        else Serial.println(": LCD not found.");
    }

    lcd.begin(16, 2); // initialize the lcd
    lcd.setBacklight(255);
}

void lcdPrint(String s) {
    int len = s.length();
    lcd.clear();
    if (len > 16) {
        len = min(len, 32);
        String s1, s2;
        for (int i = 0; i < 16; i++) s1[i] = s[i]; s1[16] = '\0';
        for (int i = 16; i < len; i++) s2[i-16] = s[i]; s2[len-15] = '\0';
        lcd.setCursor(0, 0); lcd.print(s1);
        lcd.setCursor(0, 1); lcd.print(s2);
    }
    else {
        lcd.setCursor(0, 0);
        lcd.print(s);
    }
}

void welcomeScreen(){
  lcd.setCursor(0, 0); lcd.print("    Welcome     ");
  lcd.setCursor(0, 1); lcd.print("   I'm Online   ");
  // String s = "   I'm Online " + String(failToConnect);
  // lcd.setCursor(0, 1); lcd.print(s);
}

void endScreen(){
  lcd.setCursor(0, 0); lcd.print(" Session Ended  ");
  lcd.setCursor(0, 1); lcd.print("   Thank You    ");
}

void deniedScreen() {
  lcd.setCursor(0, 0); lcd.print(" Access Denied  ");
  lcd.setCursor(0, 1); lcd.print(" Contact Admin  ");
}

void connectionLossScreen() {
    lcd.setCursor(0, 0); lcd.print("   Unexpected   ");
    lcd.setCursor(0, 1); lcd.print("Connection Loss ");
}

void unknownCardScreen() {
    lcd.setCursor(0, 0); lcd.print("    New Card    ");
    lcd.setCursor(0, 1); lcd.print("    Detected    ");
}

void unconnectedToBroker(){
    lcd.setCursor(0, 0); lcd.print("     Can't      ");
    lcd.setCursor(0, 1); lcd.print("Connect toServer");
    delay(1000);
}

void certifiedScreen(String timeLeft, String powerMeter) {
  lcd.setCursor(0, 0); lcd.print("TimeLeft: " + timeLeft + "Min ");
  lcd.setCursor(0, 1); lcd.print("Energy(Wh):" + powerMeter);
}

void pzemScreen(String timeLeft, float v, float i, float p, float e, int failToConnect) {
    char buffer4[4];
    String voltages = dtostrf(v , 3, 1, buffer4);
    String currents = dtostrf(i , 2, 2, buffer4);
    String powers = dtostrf(p , 3, 1, buffer4);
    String energys = dtostrf(e , 4, 0, buffer4);
    String failCount = String(failToConnect);

    lcd.setCursor(0, 0); lcd.print(timeLeft + " V:" + voltages + " I:" + currents);
    lcd.setCursor(0, 1); lcd.print("P:" + powers + " E:" + energys + failCount);
}
