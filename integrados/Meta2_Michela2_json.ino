#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <JKSButton.h>
#include <ArduinoJson.h>
#include <IRremote.hpp>

MCUFRIEND_kbv tela;
TouchScreen touch(6, A1, A2, 7, 300);
//JKSButton botao_RFID, botao_Infra, botao_Grava_RFID, botao_Copia_RFID, botao_Grava_Infra, botao_Emite_Infra, botaoRFID_Lista_ex, botaoInfra_Lista_ex, botao_Termina_3_1_2, botao_on, botao1, botao2, botao3, botao5, botao6, botao9, botao0, botao_cancelletto, botao_ch_plus, botao4, botao7, botao8, botao_asterisco, botao_up, botao_ok, botao_red, botao_green, botao_blue, botao_yellow, botao_down, botao_right, botao_left, botao_ch_minus, botao_vol_minus, botao_vol_plus;

//JKSButton all_buttons[] = { button_1, button_2, button_3, button_4, button_5, button_6, button_7, button_8, button_9, button_10, button_11, button_12, button_13, button_14, button_15, button_16, button_17, button_18, button_19, button_20, button_21, button_22, button_23, button_24, button_25, button_26, button_27 };
JKSButton all_buttons[27];
//void (*functionList[])() = { function_1, function_2, function_3, function_4, function_5, function_6, function_7, function_8, function_9, function_10, function_11, function_12, function_13, function_14, function_15, function_16, function_17, function_18, function_19, function_20, function_21, function_22, function_23, function_24, function_25, function_26, function_27 };

unsigned long all_hex_addresses[27];  //store all addresses from LayoutRemote to later use it to send IR signal

#define IR_SEND_PIN 21
#define IR_RECEIVE_PIN 20

// Allocate the JSON document
char* stringLayoutGeneral = "[{\"tecla\":\"on/off\",\"x\":205, \"y\":25},"    //lista de structs
"{\"tecla\":\"1\",\"x\":25, \"y\":25}," 
"{\"tecla\":\"2\",\"x\":70, \"y\":25},"
"{\"tecla\":\"3\",\"x\":115, \"y\":25},"
"{\"tecla\":\"ch+\",\"x\":160, \"y\":70},"
"{\"tecla\":\"ch-\",\"x\":160, \"y\":115}," 
"{\"tecla\":\"V+\",\"x\":160, \"y\":160},"
"{\"tecla\":\"V-\",\"x\":160, \"y\":205},"
"{\"tecla\":\"ch+\",\"x\":160, \"y\":70},"
"{\"tecla\":\"vermelha\",\"x\":205, \"y\":70}," 
"{\"tecla\":\"verde\",\"x\":205, \"y\":115},"
"{\"tecla\":\"azul\",\"x\":205, \"y\":160},"
"{\"tecla\":\"amarela\",\"x\":205, \"y\":205},"
"{\"tecla\":\"4\",\"x\":25, \"y\":70}," 
"{\"tecla\":\"5\",\"x\":70, \"y\":70},"
"{\"tecla\":\"6\",\"x\":115, \"y\":70},"
"{\"tecla\":\"7\",\"x\":25, \"y\":115},"
"{\"tecla\":\"8\",\"x\":70, \"y\":115},"
"{\"tecla\":\"9\",\"x\":115, \"y\":115},"
"{\"tecla\":\"0\",\"x\":70, \"y\":160},"
"{\"tecla\":\"#\",\"x\":115, \"y\":160},"
"{\"tecla\":\"*\",\"x\":25, \"y\":160},"
"{\"tecla\":\"^\",\"x\":70, \"y\":205},"
"{\"tecla\":\"?\",\"x\":70, \"y\":295},"  //COME METTO ^ VERSO IL BASSO?
"{\"tecla\":\"<\",\"x\":25, \"y\":250},"
"{\"tecla\":\">\",\"x\":115, \"y\":250},"
"{\"tecla\":\"ok\",\"x\":70, \"y\":250}]";


char* stringLayoutRemote = "[[\"1\", 12907897600]," //first is the name of the tecla, second is the address
"[\"2\", 2941320960],"
"[\"3\", 4010868480],"
"[\"4\", 2841050880],"
"[\"5\", 2874474240],"
"[\"6\", 3944021760],"
"[\"7\", 2974744320],"
"[\"8\", 3008167680],"
"[\"9\", 4077715200],"
"[\"0\", 4027580160],"
"[\"on/off\", 2790915840],"
"[\"ok\", 3843751680],"
"[\"V+\", 4010868480]," //not real address just for testing
"[\"V-\", 4010868480],"//not real address just for testing
"[\"ch+\", 4010868480]," //not real address just for testing
"[\"ch-\", 4010868480]," //not real address just for testing
"[\"ok\", 4010868480]," //not real address just for testing
"[\"^\", 4177985280],"
"[\">\", 3827040000],"
"[\"<\", 2774204160],"
"[\"?\", 3910598400],"
"[\"verde\", 4194692864],"
"[\"amarela\", 4077711104],"
"[\"azul\", 4177981184],"
"[\"vermelha\",4211404544]]";


int totalBotoes = 0;



JsonDocument layoutGeneral;
JsonDocument layoutRemote;

void setup() {

  Serial.begin(9600);
  IrSender.begin(IR_SEND_PIN);

  DeserializationError error1 = deserializeJson(layoutGeneral, stringLayoutGeneral);

  //Test if parsing succeeds.
  if (error1) {
    Serial.print(F("deserializeJson() 1 failed: "));
    Serial.println(error1.f_str());
    return;
  }

  DeserializationError error2 = deserializeJson(layoutRemote, stringLayoutRemote);

  // Test if parsing succeeds.
  if (error2) {
    Serial.print(F("deserializeJson() 2 failed: "));
    Serial.println(error2.f_str());
    return;
  }



  tela.begin(tela.readID());
  tela.fillScreen(TFT_BLACK);


  //botao_Termina_3_1_2.init(&tela, &touch, 190, 310, 120, 30, TFT_BLACK, TFT_RED, TFT_WHITE, "Terminar", 2);

  /* botao_on.init(&tela, &touch, 205, 25, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "on/off", 1);
  botao1.init(&tela, &touch, 25, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "1", 2);
  botao2.init(&tela, &touch, 70, 25, 40, 40, TFT_BLACK, TFT_WHITE, TFT_BLACK, "2", 2);
  botao3.init(&tela, &touch, 115, 25, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "3", 2);
  botao_ch_plus.init(&tela, &touch, 160, 70, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "ch+", 2);
  botao_ch_minus.init(&tela, &touch, 160, 115, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "ch-", 2);
  botao_vol_plus.init(&tela, &touch, 160, 160, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "V+", 2);
  botao_vol_minus.init(&tela, &touch, 160, 205, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "V-", 2);
  botao_red.init(&tela, &touch, 205, 70, 40, 40, TFT_BLACK, TFT_RED,TFT_RED, ".", 2);
  botao_green.init(&tela, &touch, 205, 115, 40, 40, TFT_BLACK, TFT_GREEN,TFT_GREEN, ".", 2);
  botao_blue.init(&tela, &touch, 205, 160, 40, 40, TFT_BLACK, TFT_BLUE,TFT_BLUE, ".", 2);
  botao_yellow.init(&tela, &touch, 205, 205, 40, 40, TFT_BLACK, TFT_YELLOW,TFT_YELLOW, ".", 2);
  botao4.init(&tela, &touch, 25, 70, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "4", 2);
  botao5.init(&tela, &touch, 70, 70, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "5", 2);
  botao6.init(&tela, &touch, 115, 70, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "6", 2);
  botao7.init(&tela, &touch, 25, 115, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "7", 2);
  botao8.init(&tela, &touch, 70, 115, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "8", 2);
  botao9.init(&tela, &touch, 115, 115, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "9", 2);
  botao0.init(&tela, &touch, 70, 160, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "0", 2);
  botao_cancelletto.init(&tela, &touch, 115, 160, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "#", 2);
  botao_asterisco.init(&tela, &touch, 25, 160, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "*", 2);
  botao_up.init(&tela, &touch, 70, 205, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "^", 2);
  botao_down.init(&tela, &touch, 70, 295, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "^", 2);
  botao_left.init(&tela, &touch, 25, 250, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "<", 2);
  botao_right.init(&tela, &touch, 115, 250, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, ">", 2);
  botao_ok.init(&tela, &touch, 70, 250, 40, 40, TFT_BLACK, TFT_WHITE,TFT_BLACK, "ok", 2);
*/


  
  for (int i = 0; i < layoutGeneral.size(); i++)  //se gli elementi sono 4 devo mettere i<4 o i<5??  i<4 perchè la posizione a cui ci riferiamo inizia a contare da 0
  {
    for (int j = 0; j < layoutRemote.size(); j++)  //IS IT OK IF I PUT <27 EVEN IF THERE ARE LESS ELEMENTS? OTHERWISE I NEED TO FIND A WAY TO COUNT THE ELEMENTS in stringLayoutRemote  j< number_elements_remote
    {
      String tecla_layoutGeneral = layoutGeneral[i]["tecla"];
      String tecla_layoutRemote = layoutRemote[j][0];
      //layoutRemote[j][0]

      //Serial.println(tecla_layoutGeneral + " " + tecla_layoutRemote + ".");

      if (tecla_layoutGeneral == tecla_layoutRemote) {
        

        int x = layoutGeneral[i]["x"];
        int y = layoutGeneral[i]["y"];
        //String display = layoutGeneral[i]["display"];
        //all_hex_addresses[n] = layoutRemote[j]["Address"];  //this would store it as a decimal value, not hexadecimal
        //IS THIS TRUE? When passing the value to IrSender.sendNECRaw, the function doesn't care whether you originally defined the value as decimal or hexadecimal—it treats them the same internally

        unsigned long address = address = layoutRemote[j][1];
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
        all_buttons[totalBotoes].setPressHandler(teste);
        

        totalBotoes++;
      }
    }

    
  }

  Serial.println(all_buttons[0].parameter);


  for (int n = 0; n < 27; n++) {
    //all_buttons[n].setPressHandler(functionList[n]);
  }  //CAN I DO THIS?
}

void teste(JKSButton & button) {
  Serial.println(button.parameter);
  IrSender.sendNECRaw(button.parameter, 0);
}


//uint8_t sCommand = 0x4;  //0x19
//uint8_t sRepeats = 0;


void loop() {
  //botao_Termina_3_1_2.process();

  for (int n = 0; n < totalBotoes; n++) {
    //Serial.println(all_buttons[n].parameter);
    all_buttons[n].process();
  }
}


//AGGIUNGI FUNZIONE PER IL BOTTONE TERMINAR



/*
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 4; j++){
      if(lista1[i]["nome"] == lista2[j]["nome"]){
        uhul
      }
    }
  }*/