#include <JKSButton.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

String listaLetras[27] ={"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z", "ENTER"};
String textoFinal = "";
JKSButton listaBotoes[27];
MCUFRIEND_kbv tela;
TouchScreen touch(6,A1,A2,7,300);
int y = 32;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tela.begin(tela.readID());
  tela.fillScreen(TFT_BLACK);
  for(int i = 0; i <27; i++){
    listaBotoes[i].parameter = i;
    int x = 20*i;

    if(i % 6 == 1){
      y+=32;
    }
    listaBotoes[i].init(&tela,&touch,x,y,40,64,TFT_BLACK,TFT_WHITE,TFT_RED,listaLetras[i].c_str(),2);
    listaBotoes[i].setPressHandler(apertarBotao);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int n = 0; n < 27; n++){
    listaBotoes[n].process();
  }
}

void apertarBotao(JKSButton& button){
  int tecla = button.parameter;
  if(tecla == 27){
    Serial.println(textoFinal);
    textoFinal = "";
  }
  else{
    textoFinal = textoFinal + listaLetras[tecla];
    Serial.println(tecla);
  }
}