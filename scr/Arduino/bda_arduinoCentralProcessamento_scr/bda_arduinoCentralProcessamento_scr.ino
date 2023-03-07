/*
  Bancada Didática de Automação

  Código de programação de plataforma (Arduino/ESP32) para o controle dos sistemas
  existentes na Bancada Didática de Automação presente no Laboratório H3 da UFG.

  Código do dispositivo de controle e processamento central do sistema.

  criado em 09 de setembro de 2021
  por Dr. Jonas Augusto Kunzler e Letícia Almeida
  modificado em ...

  Este código está disponível no endereço:
  www.github.com/...

*/

// Inclui as bibliotecas utilizadas
#include <RCSwitch.h>
#include <TimerOne.h>

// Protótipo de funções

RCSwitch mySwitch = RCSwitch();

// Define portas do arduino

int pin13 = 13;  // define entrada de sinal para acionar porta 12
int pin12 = 12;    // define porta 12
int pin11 = 11;     // define porta 11
int pin10 = 10;     // define porta 10
int pin09 = 9;  // define porta 9
int pin08 = 8;  // define porta 8
int pinAtuaFecho = 7;     // define porta 7
int pinAtuaLamp = 6;      // define porta 6
int pin05 = 5;  // define porta 5
int pin04 = 4;  // define porta 4
int pinAcionaLamp = 3;  // define porta 3
// int pin02 = 2;  // define porta 2
int pin01 = 1;  // define porta 1
int pin00 = 0;  // define porta 0

bool isAcionaLamp = LOW;
bool isMemAcionaLamp = LOW;
// bool Led09 = LOW; // define a variável booleana Led9
// bool Led12 = LOW; // define a variável booleana Led12
// bool Led13 = HIGH; // define a variável booleana Led13

// a função setup é executada uma vez quando ligada a placa ou pressionado o botão reset
void setup()
{

  // Ajuste de temporizador
  //Timer1.initialize(1000000); // Inicializa o Timer1 e configura para um período de 1 segundo

  // Ajuste do receptor 433MHz
  mySwitch.enableReceive(0);              // Recebe dados na porta 2 do arduino
  
  // -- Entradas e Saídas
//  pinMode(pinAcionaFecho, INPUT_PULLUP);  //sensor de porta
  pinMode(pinAcionaLamp, INPUT_PULLUP);   //sensor de lampada
  pinMode(pinAtuaFecho, OUTPUT);          //atuador de porta
  pinMode(pinAtuaLamp, OUTPUT);           //atuador de lampada
//  pinMode(pinTimer, OUTPUT);              //atuador de timer

  // Inicializa comunicação serial
  Serial.begin(9600);
  delay(5000);
  Serial.println("Bancada Didatica de Automacao do Lab H3");
  Serial.println("Iniciando ...");

  //saídas iniciam desligadas
  digitalWrite(pinAtuaFecho, LOW);
  digitalWrite(pinAtuaLamp, LOW);
//  digitalWrite(pinTimer, LOW);

} //end setup
/*
// Função chamada no estouro do temporizador
  void funCallback() {
  Serial.println("Fecho desativado.");
  digitalWrite(pinAtuaFecho, LOW);
  Timer1.detachInterrupt();
  }
*/
// código sequencial que se repete indefinidamente
void loop() {
  
  // Controle de acionamento de fecho eletromagnético
  if (mySwitch.available()) {
    Serial.print("Received: ");
    Serial.println(mySwitch.getReceivedValue());

      if (mySwitch.getReceivedValue() == 185642805) {
      Serial.println("Acesso liberado. Fecho ativado.");
      digitalWrite(pinAtuaFecho, HIGH);
      //digitalWrite(pinTimer, HIGH);
      delay(1000);
      Serial.println("Fecho desativado.");
      digitalWrite(pinAtuaFecho, LOW);
      //Timer1.restart();
      //Timer1.attachInterrupt(funCallback);
      //Timer1.restart();
      }
      else {
        Serial.println("Acesso negado.");
        digitalWrite(pinAtuaFecho, LOW);
      }

    mySwitch.resetAvailable();
  }

  // Controle de acionamento de lampada
  isMemAcionaLamp = isAcionaLamp;
  delay(10);
  isAcionaLamp = digitalRead(pinAcionaLamp);
  delay(10);
  if (isAcionaLamp && !isMemAcionaLamp)
  {
    digitalWrite(pinAtuaLamp, digitalRead(pinAtuaLamp) ^ 1);
    Serial.print("Status lampada: ");
    if (digitalRead(pinAtuaLamp) == HIGH) {
      Serial.println("Ligada");
    }
    else {
      Serial.println("Desligada");
    }
  }

} //end loop
