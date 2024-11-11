#include <Arduino.h>
#include <IRremote.hpp> // include the library

#define IR_RECEIVE_PIN 2

IRrecv receiver(IR_RECEIVE_PIN);
IRRawDataType rawdata=0;
uint16_t command=0;
uint16_t address=0;
decode_type_t protocol;

void setup() {
    Serial.begin(9600);
    

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    Serial.print(F("Ready to receive IR signals"));
}

void loop() {
  if (IrReceiver.decode()) {
    // Check if the protocol is known or unknown
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      rawdata=IrReceiver.decodedIRData.decodedRawData;
      Serial.println("Unknown address detected");
      Serial.print("Raw data:");
      Serial.println(rawdata);
    } else {
      protocol= IrReceiver.decodedIRData.protocol;
      Serial.print("Protocol: ");
      Serial.println(protocol);  //CAPISCI STO PROTOCOL
      command= IrReceiver.decodedIRData.command;
      Serial.print("Command: 0x");
      Serial.println(command, HEX);
      address= IrReceiver.decodedIRData.address;
      Serial.print("Address: 0x");
      Serial.println(address, HEX);
      rawdata= IrReceiver.decodedIRData.decodedRawData;
      Serial.print("Hex rawdata value: 0x");
      Serial.println(rawdata, HEX);
      
    }

    // Resume IR receiver
    IrReceiver.resume();
  }
}