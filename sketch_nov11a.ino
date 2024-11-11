#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN 5
#define SS_PIN 53

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte NEW_UID[] = { 0x3a, 0xaa, 0xaa, 0xaa };

MFRC522::MIFARE_Key key;
String content = "";
void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  if (Serial.available() > 0) {
    String escolha = Serial.readStringUntil("\n");
    escolha.trim();
    Serial.println(escolha);
    if (escolha == "1") {
      Serial.print(F("Card UID:"));
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        NEW_UID[i] = mfrc522.uid.uidByte[i];
      }
    }
    if (escolha == "2") {
      // Set new UID

      if (mfrc522.MIFARE_SetUid(NEW_UID, (byte)4, true)) {
        Serial.println(F("Wrote new UID to card."));
      }

      // Halt PICC and re-select it so DumpToSerial doesn't get confused
      mfrc522.PICC_HaltA();
      if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
      }

      // Dump the new memory contents
      Serial.print(F("New UID and contents:"));
      mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
      Serial.print(F("Card UID:"));
      for (byte i = 0; i < mfrc522.uid.size; i++) {

        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      if (mfrc522.MIFARE_SetUid(NEW_UID, (byte)4, true)) {
        Serial.println(F("Wrote new UID to card."));
      }
    }
  }
}
