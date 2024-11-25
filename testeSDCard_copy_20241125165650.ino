#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv tela;
SdFatSoftSpi<12, 11, 13> SD;

File jsonFile;

// Função para inicializar SD
void initializeSD() {
  if (!SD.begin(10)) {
    Serial.println("Erro ao inicializar o cartão SD!");
    while (true)
      ;
  }
  Serial.println("Cartão SD inicializado.");
}


void escolhainfra() {
  readJsonFile("continfra.json");
}

void escolharfid() {
  readJsonFile("contrfid.json");
}


// Função para ler o arquivo JSON e imprimir opções
void readJsonFile(const char* fileName) {
  if (SD.exists(fileName)) {
    jsonFile = SD.open(fileName);
    if (jsonFile) {
      Serial.print("Abrindo arquivo: ");
      Serial.println(fileName);
      int cont = 0;
      while (jsonFile.available()) {
        String line = jsonFile.readStringUntil('\n');
        Serial.println("Opção: " + line);
        int y = 10 + cont * 50;
        int ycursor = 15 + cont * 50;
        tela.fillRect(10, y, 140, 30, TFT_RED);
        tela.setCursor(20, ycursor);
        tela.setTextColor(TFT_BLACK);
        tela.setTextSize(2);
        tela.print(line);
        cont = cont + 1;
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
  Serial.begin(9600);
  initializeSD();
  tela.begin(tela.readID());
  tela.fillScreen(TFT_BLACK);
  escolhainfra();
}

void loop() {
}