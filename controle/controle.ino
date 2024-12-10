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
File renamedFile;
int pathlen;
IRRawDataType rawdata = 0;
unsigned long parametroAtual = 0;
String JSONWrite = "";

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
  Tela_Inicial,
  Tela_1,
  Tela_2,
  Tela_2_1,
  Tela_2_1_2,
  Tela_2_1_3,
  Tela_2_2,
  Tela_2_2_2,
  Tela_2_2_3,
  Tela_2_2_4,
  Tela_3,
  Tela_3_1,
  Tela_3_1_2,
  Tela_3_2,
  Tela_3_2_2,
  Tela_3_2_3,
  Tela_3_2_4
};

int idx_tela = Tela_Inicial;
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
  IrReceiver.begin(20, ENABLE_LED_FEEDBACK);
  IrSender.begin(21);
  initializeSD();
  tela.begin(tela.readID());
  // tela_2_2_3();
  tela_inicial();
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
  if (idx_tela == Tela_2_2) {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      Serial.println(mfrc522.PICC_IsNewCardPresent());
      Serial.println(mfrc522.PICC_ReadCardSerial());
      rfidHex = "";
      // Grava os dados no arquivo
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        rfidHex += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
        rfidHex += String(mfrc522.uid.uidByte[i], HEX);
        Serial.println(mfrc522.uid.uidByte[i]);
      }

      Serial.println("\nDados escritos em guilherme.txt");

      idx_tela = Tela_2_2_2;
      tela_2_2_2();
    }
  }
  if (idx_tela == Tela_3_2_4) {
    botao[26].setPressHandler(goto_tela_2_2_3);
    Serial.println("322");
    if (IrReceiver.decode()) {
      if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
        Serial.println(1);
        rawdata = IrReceiver.decodedIRData.decodedRawData;
      } else {
        rawdata = IrReceiver.decodedIRData.decodedRawData;
        tela.fillScreen(TFT_BLACK);
        tela.setCursor(30, 150);
        tela.println("Sinal Recebido!");
        JSONWrite += "[\"" + String(parametroAtual) + "\","+ String(rawdata) + "]," + "\n";
        tela.fillScreen(TFT_BLACK);
        tela.setCursor(30, 150);
        tela.println("Tecla Registrada!");
        delay(200);
        desenharBotoes();
      }
      Serial.println(rawdata);
      Serial.println(parametroAtual);
      IrReceiver.resume();
    }
  }
}

void desenharBotoes() {
  tela.fillScreen(TFT_BLACK);

  botao[0].init(&tela, &touch, 70, 160, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "0", 2);
  botao[1].init(&tela, &touch, 25, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "1", 2);
  botao[2].init(&tela, &touch, 70, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "2", 2);
  botao[3].init(&tela, &touch, 115, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "3", 2);
  botao[4].init(&tela, &touch, 25, 70, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "4", 2);
  botao[5].init(&tela, &touch, 70, 70, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "5", 2);
  botao[6].init(&tela, &touch, 115, 70, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "6", 2);
  botao[7].init(&tela, &touch, 25, 115, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "7", 2);
  botao[8].init(&tela, &touch, 70, 115, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "8", 2);
  botao[9].init(&tela, &touch, 115, 115, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "9", 2);
  botao[10].init(&tela, &touch, 160, 70, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ch+", 2);
  botao[11].init(&tela, &touch, 160, 115, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ch-", 2);
  botao[12].init(&tela, &touch, 70, 205, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "^", 2);
  botao[13].init(&tela, &touch, 70, 295, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "^", 2);
  botao[14].init(&tela, &touch, 25, 250, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "<", 2);
  botao[15].init(&tela, &touch, 115, 250, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, ">", 2);
  botao[16].init(&tela, &touch, 25, 160, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "*", 2);
  botao[17].init(&tela, &touch, 115, 160, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "#", 2);
  botao[18].init(&tela, &touch, 70, 250, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ok", 2);
  botao[19].init(&tela, &touch, 205, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "on/off", 1);
  botao[20].init(&tela, &touch, 160, 160, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "V+", 2);
  botao[21].init(&tela, &touch, 160, 205, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "V-", 2);
  botao[22].init(&tela, &touch, 205, 70, 40, 40, TFT_BLACK, TFT_RED, TFT_RED, ".", 2);
  botao[23].init(&tela, &touch, 205, 115, 40, 40, TFT_BLACK, TFT_GREEN, TFT_GREEN, ".", 2);
  botao[24].init(&tela, &touch, 205, 160, 40, 40, TFT_BLACK, TFT_BLUE, TFT_BLUE, ".", 2);
  botao[25].init(&tela, &touch, 205, 205, 40, 40, TFT_BLACK, TFT_YELLOW, TFT_YELLOW, ".", 2);
  botao[26].init(&tela, &touch, 205, 245, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "ENT", 2);
  for (int i = 0; i < 27; i++) {
    botao[i].setPressHandler(apertarBotaoControle);
    botao[i].parameter = i;
  }
  totalBotoesNaTela = 27;
}


void tela_inicial() {
  tela.fillScreen(TFT_BLACK);
  showBMP("logo.bmp", 60, 25);
  botao[0].init(&tela, &touch, 120, 250, 120, 75, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Iniciar", 2);
  botao[0].setPressHandler(goto_tela_1);

  totalBotoesNaTela = 1;
}

void tela_1() {
  tela.fillScreen(TFT_BLACK);
  botao[0].init(&tela, &touch, 120, 100, 200, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "RFID", 3);
  botao[1].init(&tela, &touch, 120, 240, 200, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Infrared", 3);
  botao[0].setPressHandler(goto_tela_2);
  botao[1].setPressHandler(goto_tela_3);
  totalBotoesNaTela = 2;
}

void goto_tela_1() {
  idx_tela = Tela_1;
  tela_1();
}

void goto_tela_3(JKSButton &botao_Infra) {
  idx_tela = Tela_3;
  tela_3();
}

void tela_3() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(15, 45);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Grave ou emita o \n seu Infravermelho");
  botao[0].init(&tela, &touch, 65, 160, 100, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Gravar", 2);
  botao[1].init(&tela, &touch, 180, 160, 100, 110, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Emitir", 2);
  botao[2].init(&tela, &touch, 180, 280, 90, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Voltar", 2);

  botao[2].setPressHandler(goto_tela_1);
  botao[1].setPressHandler(goto_tela_3_1);
  botao[0].setPressHandler(goto_tela_3_2);
  totalBotoesNaTela = 3;
}

void goto_tela_2(JKSButton &botao_RFID) {
  idx_tela = Tela_2;
  tela_2();
}

void tela_2() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(15, 45);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Grave ou copie o \n      seu RFID");

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
  tela.print("ID" + rfidHex + " adicionado\n    com sucesso.");
  botao[0].init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Renomear", 2);
  botao[0].setPressHandler(goto_tela_2_2_3);
}

void goto_tela_2_2_3(JKSButton &botao_Renomeia_ID) {
  idx_tela = Tela_2_2_3;
  tela_2_2_3();
}

void tela_2_2_3() {
  textoFinal = "";
  totalBotoesNaTela = 28;
  tela.fillScreen(TFT_BLACK);
  y = 20;
  x = 20;
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
  //botao[27].init(&tela, &touch, 120, y + 80, 120, 64, TFT_BLACK, TFT_WHITE, TFT_RED, textoFinal.c_str(), 2);
  totalBotoesNaTela = 27;
}



void tela_2_2_4() {
  tela.fillScreen(TFT_BLACK);
  tela.setCursor(45, 150);
  tela.setTextColor(TFT_WHITE);
  tela.setTextSize(2);
  tela.print("Nome alterado.");
  botao[0].init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                TFT_WHITE, "Voltar", 2);
  botao[0].setPressHandler(goto_tela_1);
  totalBotoesNaTela = 1;

}

void apertarBotao(JKSButton &button) {

  int tecla = button.parameter;
  
  if (tecla == 26) {
    Serial.println(textoFinal);
    if (rawdata != 0) {
      Serial.println("a");
      File meuJson = SD.open("/infra/" + textoFinal + ".json",O_CREAT | O_WRITE
      );
      meuJson.println(JSONWrite);
      meuJson.close();
    }
    else {
      renamedFile = SD.open("/rfid/" + textoFinal + ".txt", O_CREAT | O_WRITE);
      if (renamedFile) {
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          renamedFile.print(mfrc522.uid.uidByte[i]);
          renamedFile.print("\n");
        }
      } else {
        Serial.println("Erro ao abrir ou criar o arquivo guilherme.txt");
      }
      renamedFile.println();
      renamedFile.close();
    }
    tela_2_2_4();
    //botao[27].init(&tela, &touch, 120, y + 80, 120, 64, TFT_BLACK, TFT_WHITE, TFT_RED, textoFinal.c_str(), 2);

  } else {
    textoFinal = textoFinal + listaLetras[tecla];
    Serial.println(listaLetras[tecla]);
    tela.setCursor(20, 270);
    tela.setTextColor(TFT_WHITE);
    tela.setTextSize(2);
    tela.print(textoFinal);
  }
  //botao[27].init(&tela, &touch, 120, y + 80, 120, 64, TFT_BLACK, TFT_WHITE, TFT_RED, textoFinal.c_str(), 2);
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
  tela.println("Aproxime a tecla \n desejada");
  delay(200);
  desenharBotoes();
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

void apertarBotaoControle(JKSButton &botao) {
  tela.fillScreen(TFT_BLACK);
  parametroAtual = botao.parameter;
  idx_tela = Tela_3_2_4;
  totalBotoesNaTela = 0;

  tela.fillScreen(TFT_BLACK);
  tela.setCursor(30, 150);
  tela.println("Aperta a tecla \n no controle...");
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
      int y = 30 + totalBotoesNaTela * 35;
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

  Serial.println("botao");
  for (int i = 0; i < totalBotoesNaTela; i++) {
    if (&botao[i] == &button) {
      Serial.println(i);
      char caminho[30] = "";
      strcat(caminho, nomePasta);
      strcat(caminho, "/");
      strcat(caminho, caminhoarq[i]);
      Serial.println(caminho);

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

  Serial.println("botao");
  for (int i = 0; i < totalBotoesNaTela; i++) {
    if (&botao[i] == &button) {
      Serial.println(i);
      char caminho[30] = "";
      strcat(caminho, nomePasta);
      strcat(caminho, "/");
      strcat(caminho, caminhoarq[i]);
      Serial.println(caminho);
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
        Serial.println(line);
        JsonDocument layoutChunk;
        DeserializationError error2 = deserializeJson(layoutChunk, line);

        if (error2) {
          Serial.print(F("deserializeJson() 2 failed: "));
          Serial.println(error2.f_str());
          continue;
        }
        botao[0].init(&tela, &touch, 180, 300, 100, 30, TFT_BLACK, TFT_RED,
                      TFT_WHITE, "Terminar", 2);
        botao[0].setPressHandler(goto_tela_1);

        for (int i = 0; i < sizeof(lista_teclas) / sizeof(lista_teclas[0]); i++) {

          String tecla_layoutGeneral = lista_teclas[i].nome;
          String tecla_layoutRemote = layoutChunk[0];

          if (tecla_layoutGeneral == tecla_layoutRemote) {

            int x = lista_teclas[i].x;
            int y = lista_teclas[i].y;
            unsigned long address = layoutChunk[1];
            Serial.println(address);

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
              botao[totalBotoesNaTela].init(&tela, &touch, x, y, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, tecla_layoutGeneral.c_str(), 2);
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


uint8_t showBMP(char *nm, int x, int y) {
  File bmpFile;
  int bmpWidth, bmpHeight;          // W+H in pixels
  uint8_t bmpDepth;                 // Bit depth (currently must be 24, 16, 8, 4, 1)
  uint32_t bmpImageoffset;          // Start of image data in file
  uint32_t rowSize;                 // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL];  // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[(1 << PALETTEDEPTH) + BUFFPIXEL], *palette = NULL;
  uint8_t bitmask, bitshift;
  boolean flip = true;  // BMP is stored bottom-to-top
  int w, h, row, col, lcdbufsiz = (1 << PALETTEDEPTH) + BUFFPIXEL, buffidx;
  uint32_t pos;           // seek position
  boolean is565 = false;  //

  uint16_t bmpID;
  uint16_t n;  // blocks read
  uint8_t ret;

  if ((x >= tela.width()) || (y >= tela.height()))
    return 1;  // off screen

  bmpFile = SD.open(nm);             // Parse BMP header
  bmpID = read16(bmpFile);           // BMP signature
  (void)read32(bmpFile);             // Read & ignore file size
  (void)read32(bmpFile);             // Read & ignore creator bytes
  bmpImageoffset = read32(bmpFile);  // Start of image data
  (void)read32(bmpFile);             // Read & ignore DIB header size
  bmpWidth = read32(bmpFile);
  bmpHeight = read32(bmpFile);
  n = read16(bmpFile);                                         // # planes -- must be '1'
  bmpDepth = read16(bmpFile);                                  // bits per pixel
  pos = read32(bmpFile);                                       // format
  if (bmpID != 0x4D42) ret = 2;                                // bad ID
  else if (n != 1) ret = 3;                                    // too many planes
  else if (pos != 0 && pos != 3) ret = 4;                      // format: 0 = uncompressed, 3 = 565
  else if (bmpDepth < 16 && bmpDepth > PALETTEDEPTH) ret = 5;  // palette
  else {
    bool first = true;
    is565 = (pos == 3);  // ?already in 16-bit format
    // BMP rows are padded (if needed) to 4-byte boundary
    rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;
    if (bmpHeight < 0) {  // If negative, image is in top-down order.
      bmpHeight = -bmpHeight;
      flip = false;
    }

    w = bmpWidth;
    h = bmpHeight;
    if ((x + w) >= tela.width())  // Crop area to be loaded
      w = tela.width() - x;
    if ((y + h) >= tela.height())  //
      h = tela.height() - y;

    if (bmpDepth <= PALETTEDEPTH) {  // these modes have separate palette
      bmpFile.seek(BMPIMAGEOFFSET);  //palette is always @ 54
      bitmask = 0xFF;
      if (bmpDepth < 8)
        bitmask >>= bmpDepth;
      bitshift = 8 - bmpDepth;
      n = 1 << bmpDepth;
      lcdbufsiz -= n;
      palette = lcdbuffer + lcdbufsiz;
      for (col = 0; col < n; col++) {
        pos = read32(bmpFile);  //map palette to 5-6-5
        palette[col] = ((pos & 0x0000F8) >> 3) | ((pos & 0x00FC00) >> 5) | ((pos & 0xF80000) >> 8);
      }
    }

    // Set TFT address window to clipped image bounds
    tela.setAddrWindow(x, y, x + w - 1, y + h - 1);
    for (row = 0; row < h; row++) {  // For each scanline...
      // Seek to start of scan line.  It might seem labor-
      // intensive to be doing this on every line, but this
      // method covers a lot of gritty details like cropping
      // and scanline padding.  Also, the seek only takes
      // place if the file position actually needs to change
      // (avoids a lot of cluster math in SD library).
      uint8_t r, g, b, *sdptr;
      int lcdidx, lcdleft;
      if (flip)  // Bitmap is stored bottom-to-top order (normal BMP)
        pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
      else  // Bitmap is stored top-to-bottom
        pos = bmpImageoffset + row * rowSize;
      if (bmpFile.position() != pos) {  // Need seek?
        bmpFile.seek(pos);
        buffidx = sizeof(sdbuffer);  // Force buffer reload
      }

      for (col = 0; col < w;) {  //pixels in row
        lcdleft = w - col;
        if (lcdleft > lcdbufsiz) lcdleft = lcdbufsiz;
        for (lcdidx = 0; lcdidx < lcdleft; lcdidx++) {  // buffer at a time
          uint16_t color;
          // Time to read more pixel data?
          if (buffidx >= sizeof(sdbuffer)) {  // Indeed
            bmpFile.read(sdbuffer, sizeof(sdbuffer));
            buffidx = 0;  // Set index to beginning
            r = 0;
          }
          switch (bmpDepth) {  // Convert pixel from BMP to TFT format
            case 24:
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              color = tela.color565(r, g, b);
              break;
            case 16:
              b = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              if (is565)
                color = (r << 8) | (b);
              else
                color = (r << 9) | ((b & 0xE0) << 1) | (b & 0x1F);
              break;
            case 1:
            case 4:
            case 8:
              if (r == 0)
                b = sdbuffer[buffidx++], r = 8;
              color = palette[(b >> bitshift) & bitmask];
              r -= bmpDepth;
              b <<= bmpDepth;
              break;
          }
          lcdbuffer[lcdidx] = color;
        }
        tela.pushColors(lcdbuffer, lcdidx, first);
        first = false;
        col += lcdidx;
      }                                                             // end cols
    }                                                               // end rows
    tela.setAddrWindow(0, 0, tela.width() - 1, tela.height() - 1);  //restore full screen
    ret = 0;                                                        // good render
  }
  bmpFile.close();
  return (ret);
}
