/*
   Curso de Arduino e AVR WR Kits Channel
   
   Aula 90 - RFID com interface LCD
   
    
   Autor: Eng. Wagner Rambo  Data: Outubro de 2016
   
   www.wrkits.com.br | facebook.com/wrkits | youtube.com/user/canalwrkits
   
*/

// --- Bibliotecas Auxiliares ---
#include <SPI.h>
#include <MFRC522.h>                                    //Biblioteca para RFID
#include <LiquidCrystal.h>                              //Biblioteca para o display LCD
#include <TimerOne.h>
#include <RCSwitch.h>

// --- Mapeamento de Hardware ---
#define   buzzer 4                                     //buzzer no A2
#define   libera_porta 3                               //atuador da porta no A1
#define   interruptPin 2                               //sensor de transmissão de mensagem ASK

#define   SS_PIN    10
#define   RST_PIN    9

// --- Hardware do LCD ---
LiquidCrystal disp(14,  //RS no digital 8
                   15,  //EN no digital 7
                   16,  //D4 no digital 5
                   17,  //D5 no digital 4
                   18,  //D6 no digital 3
                   19); //D7 no digital 2


MFRC522 mfrc522(SS_PIN, RST_PIN);                       // Cria instância com MFRC522
RCSwitch mySwitch = RCSwitch();


// --- Protótipo das Funções Auxiliares ---
String rfid_func();                                       //Função para identificação das tags RFID
 

// --- Variáveis Globais --- 
char st[20];                                            //
String info_tag;                                        //


// --- Configurações Iniciais ---
void setup() 
{

  disp.begin(16,2);                                     //Inicializa LCD 16 x 2
  
 // -- Entradas e Saídas
  pinMode(buzzer,    OUTPUT);                           //
  pinMode(libera_porta, OUTPUT);                        //
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), callback01, RISING);
  
  Serial.begin(9600);                                   // Inicia comunicação Serial em 9600 baud rate
  SPI.begin();                                          // Inicia comunicação SPI bus
  mfrc522.PCD_Init();                                   // Inicia MFRC522
  mySwitch.enableTransmit(5);                          // 
  
  Serial.println("Aproxime o seu cartao do leitor..."); // Imprime mensagem no display LCD
  Serial.println("-");                                  // Imprime mensagem no display LCD

  //saídas iniciam desligadas
  digitalWrite(libera_porta, LOW);
  digitalWrite(buzzer,    LOW);

  disp.setCursor(0,0);                                  // Posiciona cursor na coluna 1, linha 1
  disp.print("Acesso por RFID ");                       // Imprime mensagem
   
  //Timer1.initialize(100000);                            // Inicializa temporizador e ajusta um periodo de 100ms
  //Timer1.attachInterrupt(callback01);                   // interrupção aciona chamado da função callback01
  
} //end setup

// --- Loop Infinito ---
void loop() 
{

    info_tag = rfid_func();                                   //chama função para identificação de tags RFID

} //end loop

// === Funções Auxiliares ===

void callback01()                             // Função de estouro de temporizador
{
  mySwitch.send(185642805, 28);
}

String rfid_func()                            // Função para identificação das tags RFID
{
      // -- Verifica novas tags --
      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      {
        return;
      }
      // Seleciona um dos cartões
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      {
        return;
      }
      
      // Mostra UID na serial
      Serial.print("UID da tag :");
      String conteudo = "";
      byte letra;
      for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
         Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
         Serial.print(mfrc522.uid.uidByte[i], HEX);
         conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
         conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      Serial.println();
      Serial.print("Mensagem : ");
      conteudo.toUpperCase();

      if (conteudo.substring(1) == "0B 17 74 1C")             //tag Chaveiro para liberar acesso
      {
        Serial.println("Acesso liberado!");
        Serial.println();
        
        // Libera porta e aciona buzzer
        disp.setCursor(0,1);                                 //Posiciona cursor na coluna 1, linha 2
        disp.print("Aberto: UID true");                      //Imprime mensagem 
        
        digitalWrite(buzzer,    HIGH);
        digitalWrite(libera_porta, HIGH);
        delay(1000);
      
        disp.setCursor(0,1);                                 //Posiciona cursor na coluna 1, linha 2
        disp.print("                ");                      //Imprime mensagem 
        
        digitalWrite(libera_porta, LOW);
        digitalWrite(buzzer,     LOW);

      }
     
      if (conteudo.substring(1) != "0B 17 74 1C") //outras tags
      {
        Serial.println("Acesso Negado");
        Serial.println();
        
        // Aciona   buzzer
        disp.setCursor(0,1);                                 //Posiciona cursor na coluna 1, linha 2
        disp.print("Negado UID false");                      //Imprime mensagem 
        digitalWrite(buzzer,    HIGH);
        delay(250);
        digitalWrite(buzzer,    LOW);
        delay(250);
        digitalWrite(buzzer,    HIGH);
        delay(250);
        digitalWrite(buzzer,    LOW);
        delay(250);
        digitalWrite(buzzer,    HIGH);
        delay(250);
        digitalWrite(buzzer,    LOW);
        delay(250);

        disp.setCursor(0,1);                                 //Posiciona cursor na coluna 1, linha 2
        disp.print("                ");                      //Imprime mensagem 
        
      }
  
      return conteudo;

} //end rfid_func
