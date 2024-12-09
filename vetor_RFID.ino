#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <JKSButton.h>
#include <ArduinoJson.h>
#include <IRremote.hpp>
#include <SPI.h>
#include <SdFat.h>
#include <MFRC522.h>

#define IR_SEND_PIN 21
#define IR_RECEIVE_PIN 20

#define RST_PIN 22
#define SS_PIN 53

#define SD_CS 10
#define NAMEMATCH ""
#define PALETTEDEPTH 0
#define BMPIMAGEOFFSET 54
#define BUFFPIXEL 20

char namebuf[32] = "/";

MFRC522 mfrc522(SS_PIN, RST_PIN);
byte NEW_UID[] = { 0x3b, 0xaa, 0xaa, 0xaa };

String rfidHex;
String rfidLido = "";

MFRC522::MIFARE_Key key;
String content = "";

JKSButton botao[30];
int totalBotoesNaTela = 0;
char caminhoarq[8][15];

char nomePasta[10];

MCUFRIEND_kbv tela;
SdFatSoftSpi<12, 11, 13> SD;
TouchScreen touch(6, A1, A2, 7, 300);

File jsonFile;
File diretorio;

int pathlen;

//WHAT IS THIS FOR?
uint16_t read16(File &f) {
  uint16_t result;
  f.read(&result, sizeof(result));
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  f.read(&result, sizeof(result));
  return result;
}

bool lerDoArq = true;

enum {
  Tela_1,
  Tela_2,
  Tela_2_1,
  Tela_2_1_2,
  Tela_2_1_3,
  Tela_2_2,
  Tela_2_2_2,
  Tela_2_2_3,
  Tela_3,
  Tela_3_1,
  Tela_3_1_2,
  Tela_3_2,
  Tela_3_2_2,
  Tela_3_2_3
};

int idx_tela = Tela_1;
char listaLetras[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ>";
String textoFinal = "";
int j = 0;
int x = 0;
int y = 32;


unsigned long all_hex_addresses[27];


int totalBotoes = 0;
int num_arquivo = 0;

JsonDocument layoutGeneral;
JsonDocument layoutRemote;

struct tecla {
  int x;
  int y;
  char *nome;
};

tecla lista_teclas[26] = {
  { 25, 25, "1" },
  { 205, 25, "on/off" },
  { 70, 25, "2" },
  { 115, 25, "3" },
  { 25, 70, "4" },
  { 70, 70, "5" },
  { 115, 70, "6" },
  { 25, 115, "7" },
  { 70, 115, "8" },
  { 115, 115, "9" },
  { 70, 160, "0" },
  { 115, 160, "#" },
  { 70, 205, "^" },
  { 70, 295, "?" },
  { 25, 250, "<" },
  { 115, 250, ">" },
  { 70, 250, "ok" },
  { 25, 160, "*" },
  { 160, 70, "ch+" },
  { 160, 115, "ch-" },
  { 160, 160, "V+" },
  { 160, 205, "V-" },
  { 205, 70, "vermelha" },
  { 205, 115, "verde" },
  { 205, 160, "azul" },
  { 205, 205, "amarela" }
};

void setup() {
  Serial.begin(9600);
  IrSender.begin(21);
  initializeSD();
  tela.begin(tela.readID());
  tela_1();
  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void initializeSD() {
  if (!SD.begin(10)) {
    Serial.println("Erro ao inicializar o cartão SD!");
    while (true)
      ;
  }
  Serial.println("Cartão SD inicializado.");
}


void loop() {
  // put your main code here, to run repeatedly:
  for (int n = 0; n < totalBotoesNaTela; n++) {
    botao[n].process();
  }
  
    if (idx_tela == Tela_2_1_2) {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      Serial.println("Entrou na condicao");
      if (mfrc522.MIFARE_SetUid(NEW_UID, (byte)4, true)) {
      Serial.println(F("Novo UID gravado no cartão."));
      idx_tela = Tela_2_1_3;
      tela_2_1_3();
    } else {
      Serial.println(F("Falha ao gravar o novo UID."));
    }
    mfrc522.PICC_HaltA();
    }
    
  }
}

void tela_1() {
  tela.fillScreen(TFT_BLACK);
  botao[0].init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "RFID", 2);
  botao[1].init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Infravermelho", 1);
  botao[0].setPressHandler(goto_tela_2);
  botao[1].setPressHandler(goto_tela_3);
  totalBotoesNaTela = 2;
}

void goto_tela_1(JKSButton &botao_Termina_3_1_2) {
  tela_1();
}

void goto_tela_3(JKSButton &botao_Infra) {
  idx_tela = Tela_3;
  tela_3();
}

void tela_3() {
  tela.fillScreen(TFT_BLACK);
  botao[0].init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Gravar", 2);
  botao[1].init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Emitir", 2);

  botao[1].setPressHandler(goto_tela_3_1);
  botao[0].setPressHandler(goto_tela_3_2);
  totalBotoesNaTela = 2;
}

void goto_tela_2(JKSButton &botao_RFID) {
  idx_tela = Tela_2;
  tela_2();
}

void tela_2() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(65, 250);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(1);
  tela.print("Selecione um dispositivo da lista para gravar o seu RFID");

  tela.setCursor(180, 250);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(1);
  tela.print("Copie o RFID de um dispositivo");
  botao[0].init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Gravar", 2);

  botao[1].init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Copiar", 2);
  botao[2].init(&tela, &touch, 180, 280, 90, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Voltar", 2);

  botao[0].setPressHandler(goto_tela_2_1);
  botao[1].setPressHandler(goto_tela_2_2);
  botao[2].setPressHandler(goto_tela_1);
  totalBotoesNaTela = 3;
}

void goto_tela_2_1(JKSButton &botao_Grava_RFID) {
  idx_tela = Tela_2_1;
  tela_2_1();
}

void tela_2_1() {
  readDir("/rfid");
}

/*void goto_tela_2_1_2(JKSButton &botao_Proximo_2_1_2) {
  idx_tela = Tela_2_1_2;
  lerDoArq("/guilherme.txt", NEW_UID);
  tela_2_1_2();
}
*/

void goto_tela_2_2(JKSButton &botao_Proximo_2_2) {
  idx_tela = Tela_2_2;
  tela_2_2();
}

void tela_2_1_2() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(60, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Aproxime\n   para gravar...");
  botao[0].init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Proximo", 2);
  botao[0].setPressHandler(goto_tela_2_1_3);
  totalBotoesNaTela = 1;
}

void goto_tela_2_1_3(JKSButton &botao_Renomeia_ID) {
  idx_tela = Tela_2_1_3;
  tela_2_1_3();
}

void tela_2_2() {
  totalBotoesNaTela = 1;
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(60, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Aproxime\n   para copiar...");
  botao[0].init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Proximo", 2);
  botao[0].setPressHandler(goto_tela_2_2_2);
}

void tela_2_1_3() {

  if (lerDoArq == false) {
    Serial.println(F("Erro ao ler o arquivo ou RFID inválido."));
    return;
  } else {
    Serial.print(F("Novo UID lido do arquivo: "));
    for (byte i = 0; i < 4; i++) {
      Serial.print(NEW_UID[i] < 0x10 ? " 0" : " ");
      Serial.print(NEW_UID[i], HEX);
    }
    Serial.println();
  }

  tela.fillScreen(TFT_BLACK);
  tela.setCursor(30, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("RFID registrado\n    com sucesso.");
  tela.setCursor(30, 200);
  tela.setTextColor(TFT_GREEN);
  tela.setTextSize(2);
  tela.print("RFID:");
  tela.setCursor(30, 240);
  tela.print(rfidLido);
  botao[0].init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Encerrar", 2);
  botao[0].setPressHandler(goto_tela_1);
  totalBotoesNaTela = 1;
}


void goto_tela_2_2_2(JKSButton &botao_Renomeia_ID) {
  idx_tela = Tela_2_2_2;
  tela_2_2_2();
}

void tela_2_2_2() {
  totalBotoesNaTela = 1;
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(30, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("ID x adicionado\n    com sucesso.");
  botao[0].init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Renomear", 2);
  botao[0].setPressHandler(goto_tela_2_2_3);
}

void goto_tela_2_2_3(JKSButton &botao_Renomeia_ID) {
  idx_tela = Tela_2_2_3;
  tela_2_2_3();
}

void tela_2_2_3() {
  totalBotoesNaTela = 28;
  tela.fillScreen(TFT_BLACK);
  for (int i = 0; i < 27; i++) {
    botao[i].parameter = i;
    if (i == 0) {
      x = 20;
    } else {
      x += 40;
    }
    if (x > 240) {
      x = 20;
      y += 40;
    }
    char caracter[2];
    caracter[0] = listaLetras[i];
    botao[i].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_WHITE, TFT_RED, caracter, 2);
    botao[i].setPressHandler(apertarBotao);
  }
  botao[27].init(&tela, &touch, 120, y + 80, 120, 64, TFT_BLACK, TFT_WHITE, TFT_RED, textoFinal.c_str(), 2);
}

void apertarBotao(JKSButton &button) {
  int tecla = button.parameter;
  if (tecla == 26) {
    Serial.println(textoFinal);
    textoFinal = "";
    botao[27].init(&tela, &touch, 120, y + 80, 120, 64, TFT_BLACK, TFT_WHITE, TFT_RED, textoFinal.c_str(), 2);
  } else {
    textoFinal = textoFinal + listaLetras[tecla];
    Serial.println(listaLetras[tecla]);
  }
  botao[27].init(&tela, &touch, 120, y + 80, 120, 64, TFT_BLACK, TFT_WHITE, TFT_RED, textoFinal.c_str(), 2);
}

void goto_tela_3_1(JKSButton &botao_Emite_Infra) {
  idx_tela = Tela_3_1;
  tela_3_1();
}

void tela_3_1() {
  readDir("/infra");
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
  botao[0].init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Proximo", 2);
  botao[0].setPressHandler(goto_tela_3_2_2);
  totalBotoesNaTela = 1;
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
  botao[0].init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Proximo", 2);
  botao[0].setPressHandler(goto_tela_3_2_3);
  totalBotoesNaTela = 1;
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
  botao[0].init(&tela, &touch, 160, 220, 60, 50, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Sim", 2);
  botao[1].init(&tela, &touch, 80, 220, 60, 50, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Nao", 2);
  botao[0].setPressHandler(goto_tela_3_2);
  botao[1].setPressHandler(goto_tela_1);
  totalBotoesNaTela = 2;
}


void readDir(const char *caminho) {
  strcpy(nomePasta, caminho);
  tela.fillScreen(TFT_BLACK);
  totalBotoesNaTela = 0;
  diretorio = SD.open(caminho);

  if (!diretorio) {
    Serial.print("Erro ao abrir o diretório: ");
    Serial.println(caminho);
    return;
  }
  while (true) {
    File entrada = diretorio.openNextFile();
    if (!entrada) break;
    if (totalBotoesNaTela >= 20) break;
    if (!entrada.isDirectory()) {
      entrada.getName(caminhoarq[totalBotoesNaTela], sizeof(caminhoarq[totalBotoesNaTela]));
      int y = 50 + totalBotoesNaTela * 50;
      botao[totalBotoesNaTela].init(&tela, &touch, 100, y, 140, 30, TFT_WHITE, TFT_RED, TFT_BLACK, caminhoarq[totalBotoesNaTela], 2);
      botao[totalBotoesNaTela].setPressHandler([](JKSButton &btn) {
        if (idx_tela == Tela_2_1) {
          readRFIDFile(btn);
        } else if (idx_tela == Tela_3_1) {
          readJsonFile(btn);
        }
      });
      Serial.print(F("NOME ARQUIVO BOTAO: "));
      Serial.println(caminhoarq[totalBotoesNaTela]);
      totalBotoesNaTela++;
    }
    entrada.close();
  }
  diretorio.close();
}

void readRFIDFile(JKSButton &button) {
  /*
  for (int i = 0; i < totalBotoesNaTela; i++) {
    if (&botao[i] == &button) {
      lerDoArq(caminhoarq[i], NEW_UID);
    }
  }
  */

  Serial.println("botao");
  for (int i = 0; i < totalBotoesNaTela; i++) {
    if (&botao[i] == &button) {
      Serial.println(i);
      char caminho[30] = "";
      strcat(caminho, nomePasta);
      strcat(caminho, "/");
      //  strcat(caminho, nomeArquivo[totalBotoesNaTela]);
      strcat(caminho, caminhoarq[i]);
      Serial.println(caminho);

      //jsonFile = SD.open(caminho, O_READ);
      File jsonFile = SD.open(caminho);
      if (!jsonFile) {
        Serial.println(jsonFile);
        Serial.print(F("Erro ao abrir o json: "));
        lerDoArq = false;
        return;
      }
      Serial.print(F("Abrindo arquivo: "));
      Serial.println(caminhoarq[i]);
      tela.fillScreen(TFT_BLACK);
      totalBotoesNaTela = 1;
      int i = 0;
      String rfid = "";
      while (jsonFile.available()) {
        char line[30];
        jsonFile.readBytesUntil('\n', line, sizeof(line));
        // line.trim();
        char *stopstring;
        NEW_UID[i] = strtol(line, &stopstring, 10);

        rfid += NEW_UID[i] < 0x10 ? " 0" : " ";
        rfid += String(NEW_UID[i], HEX);
        i++;
        if (i > 3) {
          break;
        }
      }
      Serial.println("RFID: " + rfid);
      jsonFile.close();
      if (rfid.length() != 12) {
        Serial.println("RFID inválido no arquivo");
        lerDoArq = false;

      } else {
        lerDoArq = true;
      }
      rfidLido = rfid;
      idx_tela = Tela_2_1_2;
      tela_2_1_2();
    }
  }
}
/*  bool lerDoArq(char *nomeArq, byte *uid) {

      char caminho[30] = "";
      strcat(caminho, "/rfid/");
      strcat(caminho, nomeArq);
      File file = SD.open(caminho);
      Serial.println(file);

      if (!file) {
        Serial.println("Erro ao abrir arquivo");
        return false;
      }
      Serial.println("Abriu arq");
      String rfid = "";
      int i = 0;
      while (file.available()) {
        String linha = file.readStringUntil('\n');
        linha.trim();
        uid[i] = (byte)linha.toInt();

        rfid += uid[i] < 0x10 ? " 0" : " ";
        rfid += String(uid[i], HEX);
        i++;
        if (i > 3) {
          break;
        }
      }
      Serial.println("RFID: " + rfid);
      file.close();
      if (rfid.length() != 12) {
        Serial.println("RFID inválido no arquivo");
        return false;
      }
      rfidLido = rfid;

      return true;
    }

    /*
  while(true){
    File entry = diretorio.openNextFile();
    if(!entry){
      break;
    }
    num_arquivo++;
  }

  char nomeArquivo[num_arquivo];
  while (true) {
    File entrada = diretorio.openNextFile();
    if (!entrada) break;
    if (totalBotoesNaTela >= 20) break;
    if (!entrada.isDirectory()) {
      entrada.getName(nomeArquivo[totalBotoesNaTela], sizeof(nomeArquivo[totalBotoesNaTela]));
      int y = 50 + totalBotoesNaTela * 50;
      botao[totalBotoesNaTela].init(&tela, &touch, 100, y, 140, 30, TFT_WHITE, TFT_RED, TFT_BLACK, nomeArquivo[totalBotoes], 2);
      botao[totalBotoesNaTela].setPressHandler([](JKSButton &btn) {
        readJsonFile(btn);
      });
      Serial.print(F("NOME ARQUIVO BOTAO: "));
      Serial.println(nomeArquivo[totalBotoesNaTela]);
      totalBotoesNaTela++;
    }
    entrada.close();
  }
  diretorio.close();
}
*/

void sendIR(JKSButton &button) {
  Serial.println(button.parameter);
  IrSender.sendNECRaw(button.parameter, 0);
}


void readJsonFile(JKSButton &button) {
  //char nomeArquivo[num_arquivo];
  Serial.println("botao");
  for (int i = 0; i < totalBotoesNaTela; i++) {
    if (&botao[i] == &button) {
      Serial.println(i);
      char caminho[30] = "";
      strcat(caminho, nomePasta);
      strcat(caminho, "/");
      //  strcat(caminho, nomeArquivo[totalBotoesNaTela]);
      strcat(caminho, caminhoarq[i]);
      Serial.println(caminho);

      //jsonFile = SD.open(caminho, O_READ);
      File jsonFile = SD.open(caminho);
      if (!jsonFile) {
        Serial.println(jsonFile);
        Serial.print(F("Erro ao abrir o json: "));
        return;
      }
      Serial.print(F("Abrindo arquivo: "));
      Serial.println(caminhoarq[i]);
      tela.fillScreen(TFT_BLACK);
      totalBotoesNaTela = 1;

      while (jsonFile.available()) {
        char line[30];
        jsonFile.readBytesUntil('\n', line, sizeof(line));
        //String line = jsonFile.readStringUntil('\n');
        Serial.println(line);
        JsonDocument layoutChunk;
        DeserializationError error2 = deserializeJson(layoutChunk, line);

        // Test if parsing succeeds.
        if (error2) {
          Serial.print(F("deserializeJson() 2 failed: "));
          Serial.println(error2.f_str());
          continue;
        }
        botao[0].init(&tela, &touch, 180, 300, 100, 30, TFT_BLACK, TFT_RED,
                      TFT_WHITE, "Terminar", 2);
        botao[0].setPressHandler(goto_tela_1);

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
              botao[totalBotoesNaTela].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, tecla_layoutGeneral.c_str(), 1);
            }

            else if (tecla_layoutGeneral == "vermelha") {
              botao[totalBotoesNaTela].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_RED, TFT_BLACK, "", 2);
            }

            else if (tecla_layoutGeneral == "verde") {
              botao[totalBotoesNaTela].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_GREEN, TFT_BLACK, "", 2);
            }

            else if (tecla_layoutGeneral == "azul") {
              botao[totalBotoesNaTela].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_BLUE, TFT_BLACK, "", 2);
            }

            else if (tecla_layoutGeneral == "amarela") {
              botao[totalBotoesNaTela].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_YELLOW, TFT_BLACK, "", 2);
            }

            else {
              botao[totalBotoesNaTela].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, tecla_layoutGeneral.c_str(), 2);  //would this work?
            }

            botao[totalBotoesNaTela].parameter = address;
            botao[totalBotoesNaTela].setPressHandler(sendIR);


            totalBotoesNaTela++;
          }
        }
      }
      jsonFile.close();
      Serial.println("foi");
    }
  }
}
