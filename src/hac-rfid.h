#include <MFRC522.h>

MFRC522 rfid(MFRC522_SS_PIN, MFRC522_RST_PIN);
MFRC522::MIFARE_Key key;

void setupRFID(){
    SPI.begin();
    pinMode(MFRC522_SS_PIN, OUTPUT);
    rfid.PCD_Init();
    rfid.PCD_SetAntennaGain(rfid.RxGain_max);
    digitalWrite(MFRC522_SS_PIN, HIGH);
    digitalWrite(MFRC522_SS_PIN, LOW);
}

bool getCardUID(char carduid[]){
    // Look for new cards
    if (!rfid.PICC_IsNewCardPresent()) return false;
    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial()) return false;
    for (byte i = 0; i < rfid.uid.size; i++) {
        carduid[i*2] = "0123456789ABCDEF"[rfid.uid.uidByte[i]/16];
        carduid[i*2+1] = "0123456789ABCDEF"[rfid.uid.uidByte[i]%16];
    } carduid[rfid.uid.size*2] = '\0';

    return true;
}
