#include <SD.h>
#include <SPI.h>

File myFile;

int pinoSS = 10;  // Pin 53 para Mega / Pin 10 para UNO

void setup() {  // Executado uma vez quando ligado o Arduino

  Serial.begin(9600);       // Define BaundRate
  pinMode(pinoSS, OUTPUT);  // Declara pinoSS como saída

  if (SD.begin()) {                              // Inicializa o SD Card
    Serial.println("SD Card pronto para uso.");  // Imprime na tela
  }

  else {
    Serial.println("Falha na inicialização do SD Card.");
    return;
  }

  myFile = SD.open("usina.txt", FILE_WRITE);  // Cria / Abre arquivo .txt

  if (myFile) {                                    // Se o Arquivo abrir imprime:
    Serial.println("Escrevendo no Arquivo .txt");  // Imprime na tela
    myFile.println("Usinainfo 1, 2 ,3 ...");       // Escreve no Arquivo
    myFile.close();                                // Fecha o Arquivo após escrever
    Serial.println("Terminado.");                  // Imprime na tela
    Serial.println(" ");
  }

  else {                                           // Se o Arquivo não abrir
    Serial.println("Erro ao Abrir Arquivo .txt");  // Imprime na tela
  }

  myFile = SD.open("usina.txt");  // Abre o Arquivo

  if (myFile) {
    Serial.println("Conteúdo do Arquivo:");  // Imprime na tela

    while (myFile.available()) {  // Exibe o conteúdo do Arquivo
      Serial.write(myFile.read());
    }

    myFile.close();  // Fecha o Arquivo após ler
  }

  else {
    Serial.println("Erro ao Abrir Arquivo .txt");  // Imprime na tela
  }
}

void loop() {

  // Como a função é executada somente uma vez, esta área permanece em branco
}