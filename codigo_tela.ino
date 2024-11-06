#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <JKSButton.h>

MCUFRIEND_kbv tela;
TouchScreen touch(6, A1, A2, 7, 300);
JKSButton botao_RFID, botao_Infra,
botao_Grava_RFID, botao_Copia_RFID, botao_Grava_Infra,
botao_Emite_Infra, botaoRFID_Lista_ex;

enum {
  Tela_inicial, Tela_RFID, Tela_Lista_Grava_RFID,
  Tela_Aproxima_Grava_RFID, Tela_Aproxima_Copia_RFID,
  Tela_Renomeia_RFID, Tela_Infra, Tela_Aproxima_Grava_Infra,
  Tela_Escolha_Tecla_Infra, Tela_Lista_Infra, Tela_Tecla_Emite_Infra
   };

int idx_tela = Tela_inicial;

void setup() {
  tela.begin( tela.readID() );
  tela_inicial();

}

void loop() {
  if (idx_tela == Tela_inicial) {
    botao_RFID.process();
    botao_Infra.process();
  }
  if (idx_tela == Tela_RFID) {
    botao_Grava_RFID.process();
    botao_Copia_RFID.process();
  }
  if (idx_tela == Tela_Infra) {
    botao_Grava_Infra.process();
    botao_Emite_Infra.process();
  }
  if (idx_tela == Tela_Lista_Grava_RFID) {
    botaoRFID_Lista_ex.process();
  }

}

void goto_Tela_RFID(JKSButton &botao_RFID) {
  idx_tela = Tela_RFID;
  tela_RFID();
}
void tela_RFID() {
  tela.fillScreen(TFT_BLACK);
  botao_Grava_RFID.init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Gravar", 2);
  botao_Copia_RFID.init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Copiar", 2);
}

void goto_Tela_Infra(JKSButton &botao_Infra) {
  idx_tela = Tela_Infra;
  tela_Infra();
}
void tela_Infra() {
  tela.fillScreen(TFT_BLACK);
  botao_Grava_Infra.init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Gravar", 2);
  botao_Emite_Infra.init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Emitir", 2);
}

void goto_Tela_Lista_Grava_RFID(JKSButton &botao_Grava_RFID) {
  idx_tela = Tela_Lista_Grava_RFID;
  tela_Lista_Grava_RFID();
}
void tela_Lista_Grava_RFID() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(20, 100);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(4);
  tela.print("Lista de RFIDs");

  botaoRFID_Lista_ex.init(&tela, &touch, 65, 160, 50, 100, TFT_BLACK, TFT_RED,
TFT_WHITE, "RFID ex...", 2);
}


void tela_inicial() {
  
  tela.fillScreen(TFT_BLACK);
  botao_RFID.init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "RFID", 2);

  botao_Infra.init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Infravermelho", 1);
  botao_RFID.setPressHandler(goto_Tela_RFID);
  botao_Infra.setPressHandler(goto_Tela_Infra);
}
