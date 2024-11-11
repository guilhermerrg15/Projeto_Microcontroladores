#include <Arduino.h>
#include <IRremote.hpp>  // include the library
#include <ArduinoJson.h>

#define IR_RECEIVE_PIN 2

IRrecv receiver(IR_RECEIVE_PIN);
IRRawDataType rawdata = 0;
uint16_t command = 0;
uint16_t address = 0;
decode_type_t protocol;
bool pegar_nome_tecla = true;

JsonDocument doc;
JsonDocument documentos;

void setup() {
  Serial.begin(9600);


  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println(F("Digite o nome da tecla"));


}

void loop() {
  //Serial.println("Digite o nome da tecla:");

  if (Serial.available() > 0) {
    String texto = Serial.readStringUntil('\n');
    texto.trim();
    if (pegar_nome_tecla == true && texto.startsWith("tecla ")) {
      String nome_tecla = texto.substring(6);
      String tecla = "tecla " + nome_tecla;

      doc["tecla"] = tecla;
      pegar_nome_tecla = false;
      Serial.println("Aperte o codigo dessa tecla");
    }
  }


  if (IrReceiver.decode()) {



    if (pegar_nome_tecla == false) {
      
      // Check if the protocol is known or unknown
      if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
        rawdata = IrReceiver.decodedIRData.decodedRawData;
        Serial.println("Unknown address detected");
        Serial.print("Raw data:");
        Serial.println(rawdata);
        doc["Raw data of Unknown protocol"] = rawdata;

      } else {
        command = IrReceiver.decodedIRData.command;
        Serial.print("Command: 0x");
        Serial.println(command, HEX);
        doc["Command"] = command;
        address = IrReceiver.decodedIRData.address;
        Serial.print("Address: 0x");
        Serial.println(address, HEX);
        doc["Address"] = address;
        rawdata = IrReceiver.decodedIRData.decodedRawData;
        Serial.print("Hex rawdata value: 0x");
        Serial.println(rawdata, HEX);
        doc["Raw data"] = rawdata;
      }
      documentos.add(doc);
      Serial.println("Tecla added to json documentos");
      serializeJsonPretty(documentos, Serial);

      // Resume IR receiver
      pegar_nome_tecla = true;
      IrReceiver.resume();
      
      Serial.println("Digite o nome da tecla:");
    }
  }
}

