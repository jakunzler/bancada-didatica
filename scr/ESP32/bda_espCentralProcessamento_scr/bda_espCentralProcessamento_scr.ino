/*
  Bancada Didática de Automação

  Código de programação da plataforma (Arduino/ESP32) para o controle dos sistemas
  existentes na Bancada Didática de Automação desenvolvida no contexto de
  Trabalho de Conclusão de Curso de Letícia Almeida.

  Código do dispositivo de controle e processamento central do sistema.

  criado em 09 de setembro de 2021
  por Dr. Jonas Augusto Kunzler e Letícia Almeida
  modificado em ...

  Repositório do projeto disponível no endereço:
  https://www.github.com/jakunzler/bancada-didatica

*/

// Inclui as bibliotecas utilizadas
#include <RCSwitch.h>
#include <AppFernandoK.h>

// SSID e senha da rede WiFi. Modifique de acordo com a sua rede.
#define SSID "LaboratorioH3"
#define PASSWORD "TelecomH3"

// Configura temporizador
hw_timer_t * temporizador = NULL;

// Objeto que irá gerenciar as conexões com os clients
AppFernandoK app;

// Protótipo de funções
RCSwitch mySwitch = RCSwitch();

// Define os pinos da plataforma
int pinAcionaFecho = 0;                                               // define pino 0 como acionador do fecho magnético
int pinAcionaLamp = 2;                                                // define pino 2 como acionador/interruptor da lâmpada no socket 01
int pinSensorNivel_A = 4;                                             // define pino 4 como sensor de nível analógico
int pinSensorNivel_D = 5;                                             // define pino 5 como sensor de nível digital
int pinLampada = 15;                                                  // define pino 15 como atuador da lâmpada no relé 01
int pinCamera = 16;                                                   // define pino 16 como atuador da câmera no relé 02
int pinTomada1 = 17;                                                  // define pino 17 como atuador da tomada 1 no relé 03
int pinTomada2 = 18;                                                  // define pino 18 como atuador da tomada 2 no relé 04
int pinAtuaFecho = 19;                                                // define pino 19 como atuador do fecho magnético no relé 05
int pinRL_06 = 21;                                                    // define pino 21 como atuador do relé 06
int pinRL_07 = 22;                                                    // define pino 22 como atuador do relé 07
int pinRL_08 = 23;                                                    // define pino 23 como atuador do relé 08
int pinAcionaRL_06 = 25;                                              // define pino 25 como acionador do relé 06
int pinAcionaRL_07 = 26;                                              // define pino 25 como acionador do relé 07
int pinAcionaRL_08 = 27;                                              // define pino 25 como acionador do relé 08

// Define variáveis

int val_analogico = 500;
bool val_digital;
bool isAcionaLamp = LOW;                                              // cria variável booleana para acionamento de lâmpada
bool isMemAcionaLamp = LOW;                                           // cria variável booleana para memória de estado da lâmpada

// A função setup é executada uma vez quando ligada a placa ou pressionado o botão reset
void setup()
{
  // Inicializa comunicação serial
  Serial.begin(115200);
  Serial.println("\n\n\n\nBancada Didatica de Automacao do Lab H3");
  Serial.println("Iniciando ...");

  // Inicializa temporizador
  temporizador = timerBegin(0, 80, true);
  timerAttachInterrupt(temporizador, &onTimer, true);
  timerAlarmWrite(temporizador, 1000000, true);
  
  // Entradas e Saídas
  mySwitch.enableReceive(pinAcionaFecho);                               // Recebe os dados por meio do link 433MHz (0)
  pinMode(pinAcionaLamp, INPUT_PULLUP);                                 // Sensor de lampada (2)
  pinMode(pinSensorNivel_A, INPUT);                                     // Sensor de nível analógico(4)
  pinMode(pinSensorNivel_D, INPUT);                                     // Sensor de nível digital (5)
  pinMode(pinLampada, OUTPUT);                                          // Atuador Relé 01 (15)
  pinMode(pinCamera, OUTPUT);                                           // Atuador Relé 02 (16)
  pinMode(pinTomada1, OUTPUT);                                          // Atuador Relé 03 (17)
  pinMode(pinTomada2, OUTPUT);                                          // Atuador Relé 04 (18)
  pinMode(pinAtuaFecho, OUTPUT);                                        // Atuador Relé 05 (19)
  pinMode(pinRL_06, OUTPUT);                                            // Atuador Relé 06 (21)
  pinMode(pinRL_07, OUTPUT);                                            // Atuador Relé 07 (22)
  pinMode(pinRL_08, OUTPUT);                                            // Atuador Relé 08 (23)
  pinMode(pinAcionaRL_06, INPUT_PULLUP);                                // Acionador Relé 06 (25)
  pinMode(pinAcionaRL_07, INPUT_PULLUP);                                // Acionador Relé 07 (26)
  pinMode(pinAcionaRL_08, INPUT_PULLUP);                                // Acionador Relé 08 (27)

  // Configura estado inicial dos pinos
  digitalWrite(pinCamera, HIGH);
  digitalWrite(pinLampada, HIGH);
  digitalWrite(pinTomada1, HIGH);
  digitalWrite(pinTomada2, HIGH);
  digitalWrite(pinAtuaFecho, LOW);
  digitalWrite(pinRL_06, LOW);
  digitalWrite(pinRL_07, LOW);
  digitalWrite(pinRL_08, LOW);

  // Conecta à WiFi
  setupWiFi();

  // Inicializa um server para que os clients se conectem na porta 80
  app.beginWiFiServer(80);

  // Informa qual função deverá ser executada quando algo chegar de algum client
  app.onMessage(onMessage);
} //end setup

// Código sequencial que se repete indefinidamente
void loop() {
  timerAlarmEnable(temporizador);                                       // Habilita uso do temporizador

  // Chama a função da lib que irá fazer as verificações necessárias (nova conexão, desconexão, nova mensagem)
    app.handleConnections();
    
  // Controle de acionamento de fecho eletromagnético
  if (mySwitch.available()) {
    Serial.print("Código recebido: ");
    Serial.println(mySwitch.getReceivedValue());
    if (mySwitch.getReceivedValue() == 185642805) {
      Serial.println("Acesso liberado. Fecho ativado.");
      digitalWrite(pinAtuaFecho, HIGH);
      delay(1000);
      Serial.println("Fecho desativado.");
      digitalWrite(pinAtuaFecho, LOW);
    }
    else {
      Serial.println("Acesso negado.");
      digitalWrite(pinAtuaFecho, LOW);
    }

    mySwitch.resetAvailable();
  }

  // Controle de acionamento de lâmpada
  isMemAcionaLamp = isAcionaLamp;
  delay(10);
  isAcionaLamp = digitalRead(pinAcionaLamp);
  delay(10);
  if (isAcionaLamp && !isMemAcionaLamp)
  {
    digitalWrite(pinLampada, digitalRead(pinLampada) ^ 1);
    Serial.print("Status lampada: ");
    if (digitalRead(pinLampada) == HIGH) {
      Serial.println("Ligada");
    }
    else
    {
      Serial.println("Desligada");
    }
  }

  // Controle de atuador do plug2
  val_digital = digitalRead(pinSensorNivel_D);
  if(val_digital == LOW && digitalRead(pinTomada2) == LOW)
  {
    Serial.println("Reservatório cheio. Desligar bomba."); 
    delay(10);
    digitalWrite(pinTomada2, HIGH);
  }
} //end loop

/* 
++++++++++ Funções auxiliares ++++++++++
*/

// Função chamada pelo temporizador
void onTimer()
{
  // Controle de atuador do plug2
  digitalWrite(pinRL_08, digitalRead(pinRL_08) ^ 1);
}

// Conecta à rede WiFi
void setupWiFi() 
{
    Serial.print("Connecting to WiFi");

    //Mandamos conectar à rede WiFi
    WiFi.begin(SSID, PASSWORD);

    //Enquanto não estiver conectado
    while (WiFi.status() != WL_CONNECTED)
    {
        //Espera 500ms
        Serial.print(".");
        delay(500);
    }

    //Se chegou aqui está conectado
    Serial.println("Connected!");

    //Mostra o IP para utilizarmos na conexão
    Serial.println(WiFi.localIP().toString());
}

//Será executada toda fez que chegar algo dos clients
void onMessage(String message, IPAddress ip)
{
    //Exibe a mensagem e o IP de quem enviou
    Serial.print("Received message: ");
    Serial.print(message);
    Serial.print(" from ");
    Serial.println(ip.toString());


    //Remove eventuais espaços em branco
    message.trim();

    //Coloca a string toda em maiúsculo
    message.toUpperCase();

    //Verifica qual mensagem que chegou e atua
    if(message == "CAMERA ON")
    {
        //Liga o relê
        digitalWrite(pinCamera, LOW);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
        //Alternativamente se quiser enviar para todos os clients conectados:
        //app.sendMessageToAll(message + " OK");
    }
    else if(message == "CAMERA OFF")
    {
        //Desliga o relê
        digitalWrite(pinCamera, HIGH);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
    }
    else if(message == "LAMP ON")
    {
        //Liga o relê
        digitalWrite(pinLampada, LOW);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
        //Alternativamente se quiser enviar para todos os clients conectados:
        //app.sendMessageToAll(message + " OK");
    }
    else if(message == "LAMP OFF")
    {
        //Desliga o relê
        digitalWrite(pinLampada, HIGH);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
    }
    else if(message == "PLUG1 ON")
    {
        //Liga o relê
        digitalWrite(pinTomada1, LOW);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
        //Alternativamente se quiser enviar para todos os clients conectados:
        //app.sendMessageToAll(message + " OK");
    }
    else if(message == "PLUG1 OFF")
    {
        //Desliga o relê
        digitalWrite(pinTomada1, HIGH);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
    }
    else if(message == "PLUG2 ON")
    {
        //Liga o relê
        digitalWrite(pinTomada2, LOW);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
        //Alternativamente se quiser enviar para todos os clients conectados:
        //app.sendMessageToAll(message + " OK");
    }
    else if(message == "PLUG2 OFF")
    {
        //Desliga o relê
        digitalWrite(pinTomada2, HIGH);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
    }
    else if(message == "LOCK ON")
    {
        //Liga o relê
        digitalWrite(pinAtuaFecho, HIGH);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
        //Alternativamente se quiser enviar para todos os clients conectados:
        //app.sendMessageToAll(message + " OK");
        delay(1000);
        digitalWrite(pinAtuaFecho, LOW);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage("LOCK OFF", ip);
    }
    else if(message == "RL_06 ON")
    {
        //Liga o relê
        digitalWrite(pinRL_06, LOW);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
        //Alternativamente se quiser enviar para todos os clients conectados:
        //app.sendMessageToAll(message + " OK");
    }
    else if(message == "RL_06 OFF")
    {
        //Desliga o relê
        digitalWrite(pinRL_06, HIGH);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
    }
    else if(message == "RL_07 ON")
    {
        //Liga o relê
        digitalWrite(pinRL_07, LOW);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
        //Alternativamente se quiser enviar para todos os clients conectados:
        //app.sendMessageToAll(message + " OK");
    }
    else if(message == "RL_07 OFF")
    {
        //Desliga o relê
        digitalWrite(pinRL_07, HIGH);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
    }
    else if(message == "RL_08 ON")
    {
        //Liga o relê
        digitalWrite(pinRL_08, LOW);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
        //Alternativamente se quiser enviar para todos os clients conectados:
        //app.sendMessageToAll(message + " OK");
    }
    else if(message == "RL_08 OFF")
    {
        //Desliga o relê
        digitalWrite(pinRL_08, HIGH);
        //Avisa para quem enviou a mensagem que tudo ocorreu bem
        app.sendMessage(message + " OK", ip);
    }
    else
    {   
        //Se chegou aqui significa que a mensagem não era conhecida
        //Então avisamos quem enviou que a mensagem não foi reconhecida
        app.sendMessage("Unknown command: " + message, ip);
    }
}
