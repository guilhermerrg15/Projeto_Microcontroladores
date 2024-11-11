#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <JKSButton.h>

MCUFRIEND_kbv tela;
TouchScreen touch(6, A1, A2, 7, 300);
JKSButton botao_RFID, botao_Infra,
botao_Grava_RFID, botao_Copia_RFID, botao_Grava_Infra,
botao_Emite_Infra, botaoRFID_Lista_ex, botaoInfra_Lista_ex, botao_Termina_3_1_2;

enum {
  Tela_1, Tela_2, Tela_2_1,
  Tela_Aproxima_Grava_RFID, Tela_Aproxima_Copia_RFID,
  Tela_Renomeia_RFID, Tela_3, Tela_Aproxima_Grava_Infra,
  Tela_Escolha_Tecla_Infra, Tela_3_1, Tela_3_1_2
   };

int idx_tela = Tela_1;

void setup() {
  tela.begin( tela.readID() );
  tela_1();

}

void loop() {
  if (idx_tela == Tela_1) {
    botao_RFID.process();
    botao_Infra.process();
  }
  if (idx_tela == Tela_2) {
    botao_Grava_RFID.process();
    botao_Copia_RFID.process();
  }
  if (idx_tela == Tela_3) {
    botao_Grava_Infra.process();
    botao_Emite_Infra.process();
  }
  if (idx_tela == Tela_2_1) {
    botaoRFID_Lista_ex.process();
  }
  if (idx_tela == Tela_3_1) {
    botaoInfra_Lista_ex.process();
  }
  if (idx_tela == Tela_3_1_2) {
    botao_Termina_3_1_2.process();
  }

}

void goto_tela_3_1_2(JKSButton &botao_Termina_3_1_2) {
  idx_tela = Tela_3_1_2;
  tela_3_1_2();
}

void tela_3_1_2() {
  tela.fillScreen(TFT_BLACK);
  botao_Termina_3_1_2.init(&tela, &touch, 160, 280, 150, 40, TFT_BLACK, TFT_RED,
TFT_WHITE, "Terminar", 2);
  botao_Termina_3_1_2.setPressHandler(goto_tela_1);
}

void goto_tela_3_1(JKSButton &botao_Emite_Infra) {
  idx_tela = Tela_3_1;
  tela_3_1();
}

void tela_3_1() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(20, 20);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Dispositivos\n");
  tela.print(" Infravermelho");
  botaoInfra_Lista_ex.init(&tela, &touch, 85, 80, 150, 40, TFT_BLACK, TFT_RED,
TFT_WHITE, "Disp 1...", 2);
  botaoInfra_Lista_ex.setPressHandler(goto_tela_3_1_2);
}

void goto_tela_3(JKSButton &botao_Infra) {
  idx_tela = Tela_3;
  tela_3();
}

void tela_3() {
  tela.fillScreen(TFT_BLACK);
  botao_Grava_Infra.init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Gravar", 2);
  botao_Emite_Infra.init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Emitir", 2);

  botao_Emite_Infra.setPressHandler(goto_tela_3_1);
}

void goto_tela_2_1(JKSButton &botao_Grava_RFID) {
  idx_tela = Tela_2_1;
  tela_2_1();
}
void tela_2_1() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(20, 20);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Lista de RFIDs");

  botaoRFID_Lista_ex.init(&tela, &touch, 85, 80, 150, 40, TFT_BLACK, TFT_RED,
TFT_WHITE, "RFID ex1...", 2);
}

void goto_tela_2(JKSButton &botao_RFID) {
  idx_tela = Tela_2;
  tela_2();
}
void tela_2() {
  tela.fillScreen(TFT_BLACK);
  botao_Grava_RFID.init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Gravar", 2);
  botao_Copia_RFID.init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Copiar", 2);
  botao_Grava_RFID.setPressHandler(goto_tela_2_1);
}

void goto_tela_1(JKSButton &botao_Termina_3_1_2) {
  idx_tela = Tela_1;
  tela_1();
}

void tela_1() {
  
  tela.fillScreen(TFT_BLACK);
  botao_RFID.init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "RFID", 2);

  botao_Infra.init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
TFT_WHITE, "Infravermelho", 1);
  botao_RFID.setPressHandler(goto_tela_2);
  botao_Infra.setPressHandler(goto_tela_3);
}
