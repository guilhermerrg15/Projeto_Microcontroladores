#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <JKSButton.h>
#include <SPI.h>
#include <SdFat.h>
#include <MFRC522.h>

#define RST_PIN 22
#define SS_PIN 53

MFRC522 mfrc522(SS_PIN, RST_PIN);
byte NEW_UID[] = { 0x3b, 0xaa, 0xaa, 0xaa };

String rfidHex;
String rfidLido = "";

MFRC522::MIFARE_Key key;
String content = "";

MCUFRIEND_kbv tela;
TouchScreen touch(6, A1, A2, 7, 300);
JKSButton botao_RFID, botao_Infra, botao_Grava_RFID, botao_Copia_RFID,
  botao_Grava_Infra, botao_Emite_Infra, botaoRFID_Lista_ex, botaoInfra_Lista_ex,
  botao_Proximo_2_1_2, botao_Proximo_2_2, botao_Encerra_2_1_3, botao_Renomeia_ID,
  botao_Termina_3_1_2, botao_Proximo_3_2, botao_Proximo_3_2_2, botao_Sim_3_2_3,
  botao_Nao_3_2_3, botao_Volta_Tela_1;

SdFatSoftSpi<12, 11, 13> SD;

#define SD_CS 10
#define NAMEMATCH ""
#define PALETTEDEPTH 0
#define BMPIMAGEOFFSET 54
#define BUFFPIXEL 20

char namebuf[32] = "/";

File root;
int pathlen;

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

void setup() {
  Serial.begin(9600);
  tela.begin(tela.readID());
  tela_1();

  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  bool good = SD.begin(SD_CS);
  if (!good) {
    Serial.print("cannot start SD");
    //while(1);
  }
  root = SD.open(namebuf);
  pathlen = strlen(namebuf);
}

bool lerDoArq(char *nomeArq, byte *uid) {
  File file = SD.open(nomeArq);
  if (!file) {
    Serial.println("Erro ao abrir arquivo");
    return false;
  }
  String rfid = "";
  int i = 0;
  while (file.available()) {
    String linha = file.readStringUntil("\n");
    linha.trim();
    uid[i] = linha.toInt();
    Serial.print(uid[i] < 0x10 ? " 0" : " ");
    rfid += uid[i] < 0x10 ? " 0" : " ";
    rfid += String(uid[i], HEX);
    i++;
  }
  Serial.println(rfid);
  file.close();
  if (rfid.length() != 8) {
    Serial.println("RFID inválido no arquivo");
    return false;
  }
  rfidLido = rfid;

  return true;
}

void loop() {

  //fazer showbmp em uma tela antes da tela 1 (tela de inicio)

  if (idx_tela == Tela_1) {
    // Serial.println(showBMP("logo3.bmp", 5, 5));
    botao_RFID.process();
    botao_Infra.process();
  }
  if (idx_tela == Tela_2) {
    botao_Grava_RFID.process();
    botao_Copia_RFID.process();
    botao_Volta_Tela_1.process();
  }
  if (idx_tela == Tela_2_1_2 && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (mfrc522.MIFARE_SetUid(NEW_UID, (byte)4, true)) {
      Serial.println(F("Novo UID gravado no cartão."));
    } else {
      Serial.println(F("Falha ao gravar o novo UID."));
    }
    mfrc522.PICC_HaltA();
    delay(1000);

    idx_tela = Tela_2_1_3;
    tela_2_1_3();
    botao_Proximo_2_1_2.process();
  }

  if (idx_tela == Tela_2_1_3) {
    botao_Encerra_2_1_3.process();
  }

  if (idx_tela == Tela_2_2 && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    rfidHex = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      rfidHex += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
      rfidHex += String(mfrc522.uid.uidByte[i], HEX);
    }
    idx_tela = Tela_2_2_2;
    tela_2_2_2();
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

void tela_3_1_2() {
  tela.fillScreen(TFT_BLACK);
  /////
  botao_Termina_3_1_2.init(&tela, &touch, 160, 280, 150, 30, TFT_BLACK, TFT_RED,
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
  tela.println("ID " + rfidHex + " adicionado\n    com sucesso.");
  tela.setCursor(60, 250);

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

  if (!lerDoArq("ex1.txt", NEW_UID)) {
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
  botao_Encerra_2_1_3.init(&tela, &touch, 180, 300, 120, 30, TFT_BLACK, TFT_RED,
                           TFT_WHITE, "Encerrar", 2);
  botao_Encerra_2_1_3.setPressHandler(goto_tela_1);
}

void goto_tela_2_1_2(JKSButton &botao_Proximo_2_1_2) {
  idx_tela = Tela_2_1_2;

  tela_2_1_2();
}

void tela_2_1_2() {
  //File file = SD.open("ex1.txt");
  //if (!file) {
  //  tela.fillScreen(TFT_BLACK);
  //  tela.setCursor(20, 20);
  //  tela.setTextColor(TFT_RED);
  //  tela.setTextSize(2);
  //  tela.print("Erro ao abrir o arquivo");
  //  delay(2000);
  // return;
  //}
  //String rfid = "";
  //while (file.available()) {
  // String line = file.readStringUntil('\n');
  // line.trim();
  // rfid += line;
  //}
  //file.close();
  // tela.fillScreen(TFT_BLACK);
  // tela.setCursor(20, 20);
  // tela.setTextColor(TFT_GREEN);
  // tela.setTextSize(2);
  // tela.print("RFID:");
  // tela.setCursor(20, 60);
  // tela.setTextSize(2);
  // tela.print(rfid);
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
