#include <JKSButton.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <IRremote.hpp>  // include the library
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#define IR_RECEIVE_PIN 2

IRrecv receiver(IR_RECEIVE_PIN);
MCUFRIEND_kbv tela;
TouchScreen touch(6, A1, A2, 7, 300);
String listateclas[27] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "ok", "on/off", "vermelha", "verde", "azul", "amarela", "^", "<", ">", "?", "V+", "V-", "ch+", "ch-" };
String nomecontrole;
IRRawDataType rawdata = 0;
JKSButton cadastro;
JKSButton botao_on;
JKSButton botao1;
JKSButton botao2;
JKSButton botao3;
JKSButton botao_ch_plus;
JKSButton botao_ch_minus;
JKSButton botao_vol_plus;
JKSButton botao_vol_minus;
JKSButton botao_red;
JKSButton botao_green;
JKSButton botao_blue;
JKSButton botao_yellow;
JKSButton botao4;
JKSButton botao5;
JKSButton botao6;
JKSButton botao7;
JKSButton botao8;
JKSButton botao9;
JKSButton botao0;
JKSButton botao_cancelletto;
JKSButton botao_asterisco;
JKSButton botao_up;
JKSButton botao_down;
JKSButton botao_left;
JKSButton botao_right;
JKSButton botao_ok;

JKSButton listabotoes[27] = { botao_on, botao1, botao2, botao3, botao_ch_plus, botao_ch_minus, botao_vol_plus, botao_vol_minus, botao_red, botao_green, botao_blue, botao_yellow, botao4, botao5, botao6, botao7, botao8, botao9, botao0, botao_cancelletto, botao_asterisco, botao_up, botao_down, botao_left, botao_right, botao_ok };
File file;
StaticJsonDocument<256> doc;
int contadorBotao = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tela.begin(tela.readID());
  tela.fillScreen(TFT_BLACK);
  Serial.println("Digite o nome do controle: ");
  if (Serial.available() > 0) {
    nomecontrole = Serial.readStringUntil("\n");
  }
  nomecontrole += ".json";
  File file = SD.open(nomecontrole, FILE_WRITE);
  for(int i = 0; i < 26;i++){
    listabotoes[i].parameter = i + 1;
  }
  botao_on.init(&tela, &touch, 205, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "on/off", 1);
  botao1.init(&tela, &touch, 25, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "1", 2);
  botao2.init(&tela, &touch, 70, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "2", 2);
  botao3.init(&tela, &touch, 115, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "3", 2);
  botao_ch_plus.init(&tela, &touch, 160, 70, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ch+", 2);
  botao_ch_minus.init(&tela, &touch, 160, 115, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ch-", 2);
  botao_vol_plus.init(&tela, &touch, 160, 160, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "V+", 2);
  botao_vol_minus.init(&tela, &touch, 160, 205, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "V-", 2);
  botao_red.init(&tela, &touch, 205, 70, 40, 40, TFT_BLACK, TFT_RED, TFT_RED, ".", 2);
  botao_green.init(&tela, &touch, 205, 115, 40, 40, TFT_BLACK, TFT_GREEN, TFT_GREEN, ".", 2);
  botao_blue.init(&tela, &touch, 205, 160, 40, 40, TFT_BLACK, TFT_BLUE, TFT_BLUE, ".", 2);
  botao_yellow.init(&tela, &touch, 205, 205, 40, 40, TFT_BLACK, TFT_YELLOW, TFT_YELLOW, ".", 2);
  botao4.init(&tela, &touch, 25, 70, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "4", 2);
  botao5.init(&tela, &touch, 70, 70, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "5", 2);
  botao6.init(&tela, &touch, 115, 70, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "6", 2);
  botao7.init(&tela, &touch, 25, 115, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "7", 2);
  botao8.init(&tela, &touch, 70, 115, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "8", 2);
  botao9.init(&tela, &touch, 115, 115, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "9", 2);
  botao0.init(&tela, &touch, 70, 160, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "0", 2);
  botao_cancelletto.init(&tela, &touch, 115, 160, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "#", 2);
  botao_asterisco.init(&tela, &touch, 25, 160, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "*", 2);
  botao_up.init(&tela, &touch, 70, 205, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "^", 2);
  botao_down.init(&tela, &touch, 70, 295, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "^", 2);
  botao_left.init(&tela, &touch, 25, 250, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "<", 2);
  botao_right.init(&tela, &touch, 115, 250, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, ">", 2);
  botao_ok.init(&tela, &touch, 70, 250, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ok", 2);
}

void loop() {
  if (contadorBotao < 26) {

  } else {
    cadastro.init(&tela, &touch, 120, 32, 120, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, "TODOS BOTOES ✓", 2);
    delay(3000);
    cadastro.init(&tela, &touch, 120, 32, 120, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, "CONTINUAR REG.", 2);
  }
}

void apertarBotao(JKSButton& botao) {
  String StringFFinal = "[";
  while (IrReceiver.decodedIRData.protocol == UNKNOWN) {
    rawdata = IrReceiver.decodedIRData.decodedRawData;
    cadastro.init(&tela, &touch, 120, 32, 120, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, "CADASTRANDO", 2);
    Serial.println(rawdata);
  }
  if(IrReceiver.decodedIRData.protocol != UNKNOWN){
    rawdata = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("Hex rawdata value: 0x");
    Serial.println(rawdata, HEX);
  }
  StringFFinal = StringFFinal + listateclas[botao.parameter] + "," + rawdata + "]";
  serializeJson(doc, StringFFinal);
  contadorBotao++;
}
