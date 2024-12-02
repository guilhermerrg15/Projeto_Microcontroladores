#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <JKSButton.h>
#include <ArduinoJson.h>
#include <IRremote.hpp>
#include <SPI.h>
#include <SdFat.h>

#define IR_SEND_PIN 21
#define IR_RECEIVE_PIN 20


MCUFRIEND_kbv tela;
SdFatSoftSpi<12, 11, 13> SD;
TouchScreen touch(6, A1, A2, 7, 300);
JKSButton botao_RFID, botao_Infra, botao_Grava_RFID, botao_Copia_RFID,
  botao_Grava_Infra, botao_Emite_Infra, botaoRFID_Lista_ex, botaoInfra_Lista_ex,
  botao_Proximo_2_1_2, botao_Proximo_2_2, botao_Encerra_2_1_3, botao_Renomeia_ID,
  botao_Termina_3_1_2, botao_Proximo_3_2, botao_Proximo_3_2_2, botao_Sim_3_2_3,
  botao_Nao_3_2_3, botao_Volta_Tela_1;

File jsonFile;

enum {
  Tela_1,
  Tela_2,
  Tela_2_1,
  Tela_2_1_2,
  Tela_2_1_3,
  Tela_2_2,
  Tela_2_2_2,
  Tela_3,
  Tela_3_1,
  Tela_3_1_2,
  Tela_3_2,
  Tela_3_2_2,
  Tela_3_2_3
};

int idx_tela = Tela_1;

JKSButton all_buttons[27];
unsigned long all_hex_addresses[27];


int totalBotoes = 0;

JsonDocument layoutGeneral;
JsonDocument layoutRemote;

struct tecla {
  char* nome;
  int x;
  int y;
};

tecla lista_teclas[26];

void initializeSD() {
  if (!SD.begin(10)) {
    Serial.println("Erro ao inicializar o cartão SD!");
    while (true)
      ;
  }
  Serial.println("Cartão SD inicializado.");
}

void escolhainfra() {
  desenharTeclasInfra("infra/Controle3.json");
}

void escolharfid() {
  //read json of RFID
}


// Função para ler o arquivo JSON e imprimir opções
void desenharTeclasInfra(const char* fileName) {
  if (SD.exists(fileName)) {
    jsonFile = SD.open(fileName);
    if (jsonFile) {
      Serial.print("Abrindo arquivo: ");
      Serial.println(fileName);
      int cont = 0;
      while (jsonFile.available()) {
        String line = jsonFile.readStringUntil('\n');
        Serial.println(line);
        JsonDocument layoutChunk;
        DeserializationError error2 = deserializeJson(layoutChunk, line);

        // Test if parsing succeeds.
        if (error2) {
          Serial.print(F("deserializeJson() 2 failed: "));
          Serial.println(error2.f_str());
          continue;
        }

        for (int i = 0; i < sizeof(lista_teclas) / sizeof(lista_teclas[0]); i++)  //size of gives the size in bytes so we do this to get the right size (26)
        {

          String tecla_layoutGeneral = lista_teclas[i].nome;
          //String tecla_layoutRemote = layoutRemote[j]["tecla"];
          String tecla_layoutRemote = layoutChunk[0];

          //Serial.println(tecla_layoutGeneral + " " + tecla_layoutRemote + ".");

          if (tecla_layoutGeneral == tecla_layoutRemote) {


            int x = lista_teclas[i].x;
            int y = lista_teclas[i].y;
            //String display = layoutGeneral[i]["display"];
            //all_hex_addresses[n] = layoutRemote[j]["Address"];  //this would store it as a decimal value, not hexadecimal
            //IS THIS TRUE? When passing the value to IrSender.sendNECRaw, the function doesn't care whether you originally defined the value as decimal or hexadecimal—it treats them the same internally

            unsigned long address = layoutChunk[1];
            Serial.println(address);
            //Serial.println("tecla found:");
            //Serial.println(display);
            //Serial.println(all_buttons[n]);


            if (tecla_layoutGeneral == "on/off") {
              all_buttons[totalBotoes].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, tecla_layoutGeneral.c_str(), 1);
            }

            else if (tecla_layoutGeneral == "vermelha") {
              all_buttons[totalBotoes].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_RED, TFT_BLACK, "", 2);
            }

            else if (tecla_layoutGeneral == "verde") {
              all_buttons[totalBotoes].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_GREEN, TFT_BLACK, "", 2);
            }

            else if (tecla_layoutGeneral == "azul") {
              all_buttons[totalBotoes].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_BLUE, TFT_BLACK, "", 2);
            }

            else if (tecla_layoutGeneral == "amarela") {
              all_buttons[totalBotoes].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_YELLOW, TFT_BLACK, "", 2);
            }

            else {
              all_buttons[totalBotoes].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, tecla_layoutGeneral.c_str(), 2);  //would this work?
            }

            all_buttons[totalBotoes].parameter = address;
            all_buttons[totalBotoes].setPressHandler(sendIR);


            totalBotoes++;
          }
        }

        cont++;
      }
      jsonFile.close();
    } else {
      Serial.println("Erro ao abrir o arquivo.");
    }
  } else {
    Serial.println("Arquivo não encontrado.");
  }
}


void setup() {
  //String stringLayoutRemote = ;

  Serial.begin(9600);
  IrSender.begin(IR_SEND_PIN);
  initializeSD();
  tela.begin(tela.readID());
  tela_1();


  tecla t1;
  t1.x = 25;
  t1.y = 25;
  t1.nome = "1";
  lista_teclas[0] = t1;

  tecla onoff;
  onoff.x = 205;
  onoff.y = 25;
  onoff.nome = "on/off";
  lista_teclas[1] = onoff;

  tecla t2;
  t2.x = 70;
  t2.y = 25;
  t2.nome = "2";
  lista_teclas[2] = t2;

  tecla t3;
  t3.x = 115;
  t3.y = 25;
  t3.nome = "3";
  lista_teclas[3] = t3;

  tecla t4;
  t4.x = 25;
  t4.y = 70;
  t4.nome = "4";
  lista_teclas[4] = t4;

  tecla t5;
  t5.x = 70;
  t5.y = 70;
  t5.nome = "5";
  lista_teclas[5] = t5;

  tecla t6;
  t6.x = 115;
  t6.y = 70;
  t6.nome = "6";
  lista_teclas[6] = t6;

  tecla t7;
  t7.x = 25;
  t7.y = 115;
  t7.nome = "7";
  lista_teclas[7] = t7;

  tecla t8;
  t8.x = 70;
  t8.y = 115;
  t8.nome = "8";
  lista_teclas[8] = t8;

  tecla t9;
  t9.x = 115;
  t9.y = 115;
  t9.nome = "9";
  lista_teclas[9] = t9;

  tecla t0;
  t0.x = 70;
  t0.y = 160;
  t0.nome = "0";
  lista_teclas[10] = t0;

  tecla ashtag;
  ashtag.x = 115;
  ashtag.y = 160;
  ashtag.nome = "#";
  lista_teclas[11] = ashtag;

  tecla up;
  up.x = 70;
  up.y = 205;
  up.nome = "^";
  lista_teclas[12] = up;

  tecla down;
  down.x = 70;
  down.y = 295;
  down.nome = "?";
  lista_teclas[13] = down;

  tecla left;
  left.x = 25;
  left.y = 250;
  left.nome = "<";
  lista_teclas[14] = left;

  tecla right;
  right.x = 115;
  right.y = 250;
  right.nome = ">";
  lista_teclas[15] = right;

  tecla ok;
  ok.x = 70;
  ok.y = 250;
  ok.nome = "ok";
  lista_teclas[16] = ok;

  tecla asterisco;
  asterisco.x = 25;
  asterisco.y = 160;
  asterisco.nome = "*";
  lista_teclas[17] = asterisco;

  tecla chplus;
  chplus.x = 160;
  chplus.y = 70;
  chplus.nome = "ch+";
  lista_teclas[18] = chplus;

  tecla chminus;
  chminus.x = 160;
  chminus.y = 115;
  chminus.nome = "ch-";
  lista_teclas[19] = chminus;

  tecla vplus;
  vplus.x = 160;
  vplus.y = 160;
  vplus.nome = "V+";
  lista_teclas[20] = vplus;

  tecla vminus;
  vminus.x = 160;
  vminus.y = 205;
  vminus.nome = "V-";
  lista_teclas[21] = vminus;

  tecla vermelha;
  vermelha.x = 205;
  vermelha.y = 70;
  vermelha.nome = "vermelha";
  lista_teclas[22] = vermelha;

  tecla verde;
  verde.x = 205;
  verde.y = 115;
  verde.nome = "verde";
  lista_teclas[23] = verde;

  tecla azul;
  azul.x = 205;
  azul.y = 160;
  azul.nome = "azul";
  lista_teclas[24] = azul;

  tecla amarela;
  amarela.x = 205;
  amarela.y = 205;
  amarela.nome = "amarela";
  lista_teclas[25] = amarela;

}

void loop() {
  if (idx_tela == Tela_1) {
    botao_RFID.process();
    botao_Infra.process();
  }
  if (idx_tela == Tela_2) {
    botao_Grava_RFID.process();
    botao_Copia_RFID.process();
    botao_Volta_Tela_1.process();
  }
  if (idx_tela == Tela_2_1_2) {
    botao_Proximo_2_1_2.process();
  }
  if (idx_tela == Tela_2_1_3) {
    botao_Encerra_2_1_3.process();
  }
  if (idx_tela == Tela_2_2) {
    botao_Proximo_2_2.process();
  }
  if (idx_tela == Tela_2_2_2) {
    botao_Renomeia_ID.process();
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
    for (int n = 0; n < totalBotoes; n++) {
      all_buttons[n].process();
    }
  }
  if (idx_tela == Tela_3_2) {
    botao_Proximo_3_2.process();
  }
  if (idx_tela == Tela_3_2_2) {
    botao_Proximo_3_2_2.process();
  }
  if (idx_tela == Tela_3_2_3) {
    botao_Sim_3_2_3.process();
    botao_Nao_3_2_3.process();
  }

}

void goto_tela_3_2_3(JKSButton &botao_Proximo_3_2_2) {
  idx_tela = Tela_3_2_3;
  tela_3_2_3();
}

void tela_3_2_3() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(30, 100);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Deseja pressionar \n   outra tecla?");
  botao_Sim_3_2_3.init(&tela, &touch, 160, 220, 60, 50, TFT_BLACK, TFT_RED,
                       TFT_WHITE, "Sim", 2);
  botao_Nao_3_2_3.init(&tela, &touch, 80, 220, 60, 50, TFT_BLACK, TFT_RED,
                       TFT_WHITE, "Nao", 2);
  botao_Sim_3_2_3.setPressHandler(goto_tela_3_2);
  botao_Nao_3_2_3.setPressHandler(goto_tela_1);
}

void goto_tela_3_2_2(JKSButton &botao_Proximo_3_2) {
  idx_tela = Tela_3_2_2;
  tela_3_2_2();
}

void tela_3_2_2() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(30, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Escolha a tecla");
  botao_Proximo_3_2_2.init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                           TFT_WHITE, "Proximo", 2);
  botao_Proximo_3_2_2.setPressHandler(goto_tela_3_2_3);
}

void goto_tela_3_2(JKSButton &botao_Grava_Infra) {
  idx_tela = Tela_3_2;
  tela_3_2();
}

void tela_3_2() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(30, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Aperte a tecla\n   para gravar...");
  botao_Proximo_3_2.init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                         TFT_WHITE, "Proximo", 2);
  botao_Proximo_3_2.setPressHandler(goto_tela_3_2_2);
}

void goto_tela_3_1_2(JKSButton &botao_Termina_3_1_2) {
  idx_tela = Tela_3_1_2;
  tela_3_1_2();
}

void sendIR(JKSButton &button) {
  Serial.println(button.parameter);
  IrSender.sendNECRaw(button.parameter, 0);
}

void tela_3_1_2() {
  tela.fillScreen(TFT_BLACK);
  /////
  botao_Termina_3_1_2.init(&tela, &touch, 180, 300, 100, 30, TFT_BLACK, TFT_RED,
                           TFT_WHITE, "Terminar", 2);
  botao_Termina_3_1_2.setPressHandler(goto_tela_1);

  escolhainfra();    
  
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
  botao_Grava_Infra.setPressHandler(goto_tela_3_2);
}

void goto_tela_2_2_2(JKSButton &botao_Renomeia_ID) {
  idx_tela = Tela_2_2_2;
  tela_2_2_2();
}

void tela_2_2_2() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(30, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("ID x adicionado\n    com sucesso.");
  botao_Renomeia_ID.init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                         TFT_WHITE, "Renomear", 2);
  botao_Renomeia_ID.setPressHandler(goto_tela_1);
}

void goto_tela_2_2(JKSButton &botao_Proximo_2_2) {
  idx_tela = Tela_2_2;
  tela_2_2();
}

void tela_2_2() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(60, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Aproxime\n   para copiar...");
  botao_Proximo_2_2.init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                         TFT_WHITE, "Proximo", 2);
  botao_Proximo_2_2.setPressHandler(goto_tela_2_2_2);
}

void goto_tela_2_1_3(JKSButton &botao_Renomeia_ID) {
  idx_tela = Tela_2_1_3;
  tela_2_1_3();
}

void tela_2_1_3() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(30, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("RFID registrado\n    com sucesso.");
  botao_Encerra_2_1_3.init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                           TFT_WHITE, "Encerrar", 2);
  botao_Encerra_2_1_3.setPressHandler(goto_tela_1);
}

void goto_tela_2_1_2(JKSButton &botao_Proximo_2_1_2) {
  idx_tela = Tela_2_1_2;
  tela_2_1_2();
}

void tela_2_1_2() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(60, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Aproxime\n   para gravar...");
  botao_Proximo_2_1_2.init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                           TFT_WHITE, "Proximo", 2);
  botao_Proximo_2_1_2.setPressHandler(goto_tela_2_1_3);
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
  botaoRFID_Lista_ex.setPressHandler(goto_tela_2_1_2);
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
  botao_Volta_Tela_1.init(&tela, &touch, 180, 280, 90, 30, TFT_BLACK, TFT_RED,
                          TFT_WHITE, "Voltar", 2);

  botao_Grava_RFID.setPressHandler(goto_tela_2_1);
  botao_Copia_RFID.setPressHandler(goto_tela_2_2);
  botao_Volta_Tela_1.setPressHandler(goto_tela_1);
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
