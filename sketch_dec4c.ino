#include <JKSButton.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <IRremote.hpp> // include the library
#define IR_RECEIVE_PIN 2

IRrecv receiver(IR_RECEIVE_PIN);
MCUFRIEND_kbv tela;
TouchScreen touch(6, A1, A2, 7, 300);
String listateclas = {"1","2","3","4","5","6","7","8","9","0","ok","on/off","vermelha","verde","azul","amarela","^","<",">","?","V+","V-","ch+","ch-"};
String nomecontrole;
IRRawDataType rawdata=0;
JKSButton cadastro;
int contadorBotao = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tela.begin(tela.readID());
  tela.fillScreen(TFT_BLACK);
  Serial.println("Digite o nome do controle: ")
  if(Serial.available() > 0){
    nomecontrole = Serial.readStringUntil("\n");
  }
  nomecontrole += ".json"
  File file = SD.open(nomecontrole, FILE_WRITE);
  botao_on.init(&tela, &touch, 205, 25, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "on/off", 1);
  botao1.init(&tela, &touch, 25, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "1", 2);
  botao2.init(&tela, &touch, 70, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "2", 2);
  botao3.init(&tela, &touch, 115, 25, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "3", 2);
  botao_ch_plus.init(&tela, &touch, 160, 70, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "ch+", 2);
  botao_ch_minus.init(&tela, &touch, 160, 115, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "ch-", 2);
  botao_vol_plus.init(&tela, &touch, 160, 160, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "V+", 2);
  botao_vol_minus.init(&tela, &touch, 160, 205, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "V-", 2);
  botao_red.init(&tela, &touch, 205, 70, 40, 40, TFT_BLACK, TFT_RED,TFT_RED, ".", 2);
  botao_green.init(&tela, &touch, 205, 115, 40, 40, TFT_BLACK, TFT_GREEN,TFT_GREEN, ".", 2);
  botao_blue.init(&tela, &touch, 205, 160, 40, 40, TFT_BLACK, TFT_BLUE,TFT_BLUE, ".", 2);
  botao_yellow.init(&tela, &touch, 205, 205, 40, 40, TFT_BLACK, TFT_YELLOW,TFT_YELLOW, ".", 2);
  botao4.init(&tela, &touch, 25, 70, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "4", 2);
  botao5.init(&tela, &touch, 70, 70, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "5", 2);
  botao6.init(&tela, &touch, 115, 70, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "6", 2);
  botao7.init(&tela, &touch, 25, 115, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "7", 2);
  botao8.init(&tela, &touch, 70, 115, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "8", 2);
  botao9.init(&tela, &touch, 115, 115, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "9", 2);
  botao0.init(&tela, &touch, 70, 160, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "0", 2);
  botao_cancelletto.init(&tela, &touch, 115, 160, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "#", 2);
  botao_asterisco.init(&tela, &touch, 25, 160, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "*", 2);
  botao_up.init(&tela, &touch, 70, 205, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "^", 2);
  botao_down.init(&tela, &touch, 70, 295, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "^", 2);
  botao_left.init(&tela, &touch, 25, 250, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "<", 2);
  botao_right.init(&tela, &touch, 115, 250, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, ">", 2);
  botao_ok.init(&tela, &touch, 70, 250, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "ok", 2);

}

void loop() {
  if(contadorBotao < 26){

  }else{
    cadastro.init(&tela, &touch, 120, 32, 120, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ERRO NO CADASTRO", 2);
  }
}

void apertarBotao(&JKSButton botao){
  if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      rawdata=IrReceiver.decodedIRData.decodedRawData;
      cadastro.init(&tela, &touch, 120, 32, 120, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ERRO NO CADASTRO", 2);
      Serial.println(rawdata);
    } else {
      rawdata= IrReceiver.decodedIRData.decodedRawData;
      Serial.print("Hex rawdata value: 0x");
      Serial.println(rawdata, HEX);
    }
  string stringFinal = "[";
  stringFinal = stringFinal +  listateclas[botao.parametro] + "," + rawdata + "]";
  serializeJson(stringFinal, file);
  contadorBotao++;
}
