#include <JKSButton.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

bool numdefinido = false;
int numbotoes = 0;
String textoButton = "";
int i = 0;
MCUFRIEND_kbv tela;
TouchScreen touch(6,A1,A2,7,300);
typedef struct{
  int codigo;
  String nome;
} Botao;
Botao botoes[100];
JKSButton botaoAtual;
JKSButton botaoNext;
JKSButton botaoConfirmar;
bool confirmar = false;
int y = 32;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tela.begin(tela.readID());
  tela.fillScreen(TFT_BLACK);
  botaoNext.setPressHandler(apertarBotaoNext);
  botaoNext.init(&tela, &touch, 120, 32, 120, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, "NEXT", 2);
  botaoConfirmar.init(&tela, &touch, 120, 96, 120, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, "CONFIRMAR", 2);
  botaoAtual.init(&tela, &touch, 120, 160, 120, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, textoButton.c_str(), 2);
  Serial.println("Defina o número de botões");
}

void loop() {
  
  if(Serial.available() > 0){
    numbotoes = Serial.parseInt();
    Serial.println(numbotoes);
    numdefinido = true;
  }
  botaoNext.process();
  botaoAtual.process();
  botaoConfirmar.process();
  while( i < numbotoes){
    if(Serial.available() > 0){
      Serial.println("Digite o código do botão: ");
      String codigo;
      codigo = Serial.readStringUntil('\n');
      botoes[i].codigo = codigo.toInt();
      Serial.println("Digite o nome do botão: ");
      botoes[i].nome = Serial.readStringUntil('\n');
      textoButton = "Botao " + String(i) + " - " + botoes[i].codigo;
      botaoAtual.init(&tela, &touch, 120, 160, 240, 64, TFT_BLACK, TFT_WHITE, TFT_BLACK, textoButton.c_str(), 2);
    }

  }
  }


void apertarBotaoNext(JKSButton& button){
  i++;
}

void apertarBotaoConfirmar(JKSButton& button){
  confirmar = true;
}