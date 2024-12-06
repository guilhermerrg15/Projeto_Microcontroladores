#include <JKSButton.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

char listaLetras[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ>"}
String textoFinal = "";
JKSButton listaBotoes[27];
JKSButton resultado;
MCUFRIEND_kbv tela;
TouchScreen touch(6,A1,A2,7,300);
int x = 0;
int y = 32;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tela.begin(tela.readID());
  tela.fillScreen(TFT_BLACK);
  for(int i = 0; i <26; i++){
    listaBotoes[i].parameter = i;
    if(i == 0){
      x = 20;
    }else{
      x += 40;
    }
    if(x > 240){
      x = 20;
      y +=40;
    }
    listaBotoes[i].init(&tela,&touch,x,y,40,40,TFT_BLACK,TFT_WHITE,TFT_RED,listaLetras[i].c_str(),2);

    listaBotoes[i].setPressHandler(apertarBotao);
  }
  resultado.init(&tela,&touch,120,y+80,120,64,TFT_BLACK,TFT_WHITE,TFT_RED,textoFinal.c_str(),2);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int n = 0; n < 27; n++){
    listaBotoes[n].process();
  }
  resultado.process();
  
}

void apertarBotao(JKSButton& button){
  int tecla = button.parameter;
  if(tecla == 27){
    Serial.println(textoFinal);
    textoFinal = "";
  }
  else{
    textoFinal = textoFinal + listaLetras[tecla];
    Serial.println(listaLetras[tecla]);
  }
  resultado.init(&tela,&touch,120,y+80,120,64,TFT_BLACK,TFT_WHITE,TFT_RED,textoFinal.c_str(),2);
}