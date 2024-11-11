#include <Arduino.h>

#if !defined(ARDUINO_ESP32C3_DEV) // This is due to a bug in RISC-V compiler, which requires unused function sections :-(.
#define DISABLE_CODE_FOR_RECEIVER // Disables static receiver code like receive timer ISR handler and static IRReceiver and irparams data. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not required.
#endif

/*consider the recorded IR signals:
  },
  {
    "tecla": "tecla verde",
    "Command": 5,
    "Address": 61184,
    "Raw data": 4194692864
  },
  {
    "tecla": "tecla azul",
    "Command": 6,
    "Address": 61184,
    "Raw data": 4177981184
  }
]

Hex rawdata value: 
vermelho:0xFB04EF00  command 0x4
verde: 0xFA05EF00    command 0x5
azul: 0xF906EF00     command 0x6
*/


#include <IRremote.hpp> // include the library

#define IR_SEND_PIN 3

#define IR_RECEIVE_PIN 2


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    while (!Serial)
        ; // Wait for Serial to become available. Is optimized away for some cores.

    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
    Serial.print(F("Send IR signals at pin "));
    Serial.println();

    /*
     * The IR library setup. That's all!
     */
    IrSender.begin(IR_SEND_PIN); // Start with IR_SEND_PIN -which is defined in PinDefinitionsAndMore.h- as send pin and enable feedback LED at default feedback LED pin
    disableLEDFeedback(); // Disable feedback LED at default feedback LED pin
}

/*
 * Set up the data to be sent.
 * For most protocols, the data is build up with a constant 8 (or 16 byte) address
 * and a variable 8 bit command.
 * There are exceptions like Sony and Denon, which have 5 bit address.
 */
uint8_t sCommand = 0x4; //0x19
uint8_t sRepeats = 0;

void loop() {
    /*
     * Print current send values
     */
    Serial.println();
    Serial.print(F("Send now: address=0x00, command=0x"));
    Serial.print(sCommand, HEX);
    Serial.print(F(", repeats="));
    Serial.print(sRepeats);
    Serial.println();

    Serial.println(F("Send standard NEC with 8 bit address"));
    Serial.flush();

    // Receiver output for the first loop must be: Protocol=NEC Address=0x102 Command=0x34 Raw-Data=0xCB340102 (32 bits)
    // IrSender.sendNEC(0x00, sCommand, sRepeats);

   
     // If you want to send a raw HEX value directly like e.g. 0xCB340102 you must use sendNECRaw()
     
   Serial.println(F("Send 32 bit HEX value 0xFB04EF00"));
    IrSender.sendNECRaw(0xF906EF00, sRepeats);
  delay(1000);
  IrSender.sendNECRaw(0xFA05EF00, sRepeats); //green
  delay(1000);
    IrSender.sendNECRaw(0xFB04EF00, sRepeats); //red

    /*
     * If you want to send an "old" MSB HEX value used by IRremote versions before 3.0 like e.g. 0x40802CD3 you must use sendNECMSB()
     */
//    Serial.println(F("Send old 32 bit MSB 0x40802CD3 with sendNECMSB()"));
//    IrSender.sendNECMSB(0x40802CD3, 32, sRepeats);

    /*
     * Increment send values
     */

     /*
    sCommand += 0x11;
    sRepeats++;
    // clip repeats at 4
    if (sRepeats > 4) {
        sRepeats = 4;
    }*/

    delay(2000);  // delay must be greater than 5 ms (RECORD_GAP_MICROS), otherwise the receiver sees it as one long signal
}
