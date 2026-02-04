/*
**********************   Para não ter erro de compilação   ********************** 

Boards Manager:
-> Esp32 by Espressif Versão 2.0.14

Library:
->Heltec ESP32 Dev-Boards by Heltec Automation Versão 1.1.1

*/

//*****************************   Libraries   *****************************

#include <EEPROM.h>
#include <LittleFS.h>   // para gravar dados permanentes "mais seguro do que o SPIFFS" em relação a corromper arquivos por falta de energia
#include "FS.h"
#include <WebServer.h>
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <Wire.h>
#include "RTClib.h"
//#include "Web_Front_End.h"
#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>



#include "heltec.h"

//*****************************   Defines   *****************************

//#define BAND    915E6  //Escolha a frequência radio lora  868-915MHz
#define BAND    90973E4 // (868 a 909,7) - 909,73MHz - (909,76 - 915)    faixa de 30 khz para não ter interferencia 
#define Pin_ADC_Bateria 13
#define Pin_Sensor_DTH 17
#define LED2 25
#define calibrar false  // coloque true quando quiser calibrar

//#define ssid "JRTELECOMADOLFO" // Your WiFi SSID
//#define password "38632391"    // Your WiFi Password
#define SSID "JRTELECOMADOLFO" // Your WiFi SSID
#define PASSWORD "38632391"    // Your WiFi Password
WebServer server(80);

//*****************************   Estados dos sensores (bitmask)   *****************************
#define WAIT        0b111  // repouso
#define STATE_3     0b110  // S3 foi o primeiro sensor
#define STATE_2     0b101  // S2 já aconteceu (independente do estado atual)
#define STATE_2_3   0b100  // sensor 2 e 3 acionado
#define STATE_1     0b011  // S1 foi o primeiro sensor
#define STATE_1_3   0b010  // sensor 1 e 3 acionado
#define STATE_1_2   0b001  // sensor 1 e 2 acionado
#define STATE_1_2_3 0b000  // sensor 1, 2 e 3 acionado

RTC_DS3231 rtc; // modulo rtc

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

TwoWire I2C_RTC = TwoWire(1); // I2C secundário (isolado)

//*****************************   Variaveis Externas   *****************************

extern const char PAGE_HTML[] PROGMEM; // HTML web page
extern const char PAGE_FILE_NOT_FOUND[] PROGMEM;
//extern const char index_html[] PROGMEM;
//extern String readFile(fs::FS &fs, const char * path);
extern String processor(const String& var);
//extern void notFound(AsyncWebServerRequest *request);
extern void Setup_Server();
//extern void setup_Balanca();
//extern void writeFile(fs::FS &fs, const char * path, const char * message);
extern const char* PARAM_STRING; // WEB


//PINOUT
//******   INPUTS   ****** 
const int S1 = 23; //mcu > este pino deve estar em low ao fazer o downloading do programa
const int S2 = 2; 
const int S3 = 17; 
const int button_WakeUp = 36;
const int botao_Configurar = 37;
const int Adc_Battery = 38; //mcu > este pino deve estar somente como entrada

//******   OUTPUTS   ****** 
const int SaidaVext = 21; // Vext: External power supply (3,3v), control the switch of Vext through GPIO (LOW -> ON , HIGH -> OFF)   

// RTC usa os pinos por padrão DS3231
//SCL -> GPIO22 
//SDA -> GPIO21

enum EstadoFSM 
{
  FSM_IDLE,        // aguardando início (111)
  FSM_S1_SEEN,     // S1 já foi visto
  FSM_S3_SEEN,     // S3 já foi visto
  FSM_S1_S2_SEEN,  // S1 e S2 já foram vistos
  FSM_S3_S2_SEEN,   // S3 e S2 já foram vistos
  FSM_ALL_SEEN,     // os 3 sensores já ocorreram
  FSM_RETURN
};

enum Sentido 
{
  ENTRADA = 0,
  SAIDA   = 1
};

EstadoFSM estadoFSM = FSM_IDLE;
Sentido sentidoFSM;
unsigned long tempoEstado = 0;
int Quant_timeout_state2 = 0;
const unsigned long TIMEOUT_FSM = 5000; // 5 segundos

String IDcaixa;  

//******************************************************************

const char* PARAM_INT   = "inputInt";
const char* PARAM_FLOAT = "inputFloat";

//******************************************************************

hw_timer_t * Timer_SalvarDados = NULL; // temporizador
volatile bool Flag_tempo_SalvarDados = false;

// Variables
const int     calVal_eepromAdress = 0;

uint8_t state_Sensors= 0;
bool Flag_Sensors = false;
bool State_button_WakeUp = true; // Button to ON Display

const unsigned long Time_Display = 15000; // 15s
unsigned long time_DisplayON= 0; // time the display is ON

unsigned long TempoUltimoDadoSalvo = 0;
unsigned long t = 0;
unsigned long tempo_salvarDados = 60; // em segundos  Tempo em que o timer é configurado para armazenar os dados no arquivo csv

String packet, ip_server ;
float currentBateria;

/*********************   Protótipos das funções **********************/
void sendPacket();
void IRAM_ATTR InterruptExternSensors();
void processaFSM(uint8_t estado);
void registraEvento(uint8_t ultimoEstadoFSM);
void printEstadoSensores(uint8_t estado);
unsigned int ReadtimeRTC();
uint8_t ReadingSensors();
void ReadButtonDisplay();
float ReadBattery();
void handleFileDownload();
String formatarTamanho(size_t bytes);
void SalvarDadosEmArquivo();
void DeleteDadosAtuais();

/*********************   Interrupção timer **********************/
void IRAM_ATTR Ativa_Flag_tempo_SalvarDados()   //função de envio quando a interrupção é chamada
{
  Flag_tempo_SalvarDados = true;
}
//AsyncWebServer server(80);

/******************* função principal (setup) *********************/
void setup()
{
  setCpuFrequencyMhz(160);  // 240Mhz = 140mA,   160Mhz = 119mA,  80Mhz = 105mA

  pinMode(button_WakeUp,INPUT);
  pinMode(S1,INPUT);
  pinMode(S2,INPUT);
  pinMode(S3,INPUT);
  pinMode(SaidaVext,OUTPUT);
  pinMode(botao_Configurar,INPUT);

 //*********************   Interrupção externa para os 3 sensores   **********************
  attachInterrupt(S1, InterruptExternSensors, CHANGE);
  attachInterrupt(S2, InterruptExternSensors, CHANGE);
  attachInterrupt(S3, InterruptExternSensors, CHANGE);

 //*********************   Timer para salvar dados no arquivo csv   **********************
  Timer_SalvarDados = timerBegin(1, 80, true); // ( numero do temporizador utilizado no esp tem de 0 a 3, prescaler, e o último é um sinalizador indicando se o contador deve contar para cima (verdadeiro) ou para baixo (falso) )
  timerAttachInterrupt(Timer_SalvarDados, &Ativa_Flag_tempo_SalvarDados, true);
  timerAlarmWrite(Timer_SalvarDados, tempo_salvarDados*1000000, true); // 1000000 microssegundos = 1 segundo, Para o terceiro argumento, passaremos o valor true, assim o contador irá recarregar e assim a interrupção será gerada periodicamente.
  timerAlarmEnable(Timer_SalvarDados);




  int cont =0;

  //SCL -> GPIO22 
  //SDA -> GPIO21
  //Wire.begin(23, 17, 100000); // SDA, SCL, 100kHz
  //Wire.begin(4, 15);  // SDA, SCL  ← ESSENCIAL
  //Wire.setClock(100000); // 100 kHz (RECOMENDADO) Modulo DS3231 não trabalha bem com 400khz padrão

  delay(500);
  Serial.println("************************   Inicializando Display   ************************  ");
  delay(500);
  Heltec.begin(true /*Habilita o Display*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Habilita debug Serial*/, true /*Habilita o PABOOST*/, BAND /*Frequência BAND*/);
  displaySetup();
  delay(500);

  Serial.begin(115200);  
  delay(500);
  
  Serial.flush();
  Serial.println("************************   Scan I2C   ************************   ");
  for (byte addr = 1; addr < 127; addr++) 
  {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) 
    {
      Serial.print("Encontrado em 0x");
      Serial.println(addr, HEX);
    }
    delay(10);
  }

  // I2C do RTC isolado
  I2C_RTC.begin(13, 22, 100000); // SDA, SCL, 100kHz

  digitalWrite(SaidaVext,LOW);
    delay(500);
  Serial.println("************************   Scan barramento I2C Secundario    ************************   ");
  for (byte addr = 1; addr < 127; addr++) 
  {
    I2C_RTC.beginTransmission(addr);
    if (I2C_RTC.endTransmission() == 0) 
    {
      Serial.print("Encontrado em 0x");
      Serial.println(addr, HEX);
    }
    delay(10);
  }

  // inicializando modulo rtc
  Serial.flush();
  Serial.println("************************   Inicializando modulo RTC   ************************  ");
  delay(100);

  if (!rtc.begin(&I2C_RTC)) 
  {
    Serial.println(" RTC nao encontrado ");


  } else {
    Serial.println(" RTC OK");
    DateTime now = rtc.now();
    Serial.println(now.timestamp(DateTime::TIMESTAMP_FULL));
  }

  //rtc.adjust(DateTime(2026, 1, 05, 8, 25, 0));

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(2026, 1, 5, 8, 32, 0)); //     5/01/2026  8h:32 min
  }

  if (digitalRead(button_WakeUp) == HIGH)
  {
    rtc.adjust(DateTime(2026, 1, 5, 8, 32, 0));
    Serial.println(" RTC Setado");
  }
  
  Serial.flush();
  /*
  Serial.println("************************   Inicializando SPIFFS  ************************  ");
  if (SPIFFS.begin(true)) {
    Serial.println("SPIFFS OK");
  }

 */






  /*
  if (digitalRead(botao_Configurar) == true)
  {
    display_Modo_Configuracao();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      display_WiFi_Failed(cont);
      WiFi.begin(ssid, password);
      cont++;
      delay(3000);
    }
    display_Wifi_OK();
    ip_server = WiFi.localIP().toString();
    display_IPServidor();
    Setup_Server();
    while(true){
      display_IPServidor();
      Serial.println("\nModo Configuração ativado, para sair desligue a chave de configuração e reset a placa !\n");
    }
  }
  IDcaixa = GET_ID_CAIXA();
  */
    BeginLittleFS();

    Serial.println("\nConectando ao WiFi...");
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi conectado!");
    Serial.print("IP local do ESP32: ");
    Serial.print(WiFi.localIP());
    Serial.println("\n");

    // Página principal
    server.on("/", HTTP_GET, []() {
    String page = PAGE_HTML;                 // copia do PROGMEM
    page.replace("<!-- FILE_LIST -->", gerarHTMLArquivos());
    server.send(200, "text/html", page);
    });

    server.on("/download", HTTP_GET, handleFileDownload);

    server.begin();
    Serial.println("Servidor HTTP iniciado");
}

// *****************************   Variables Global   *****************************

unsigned int EntradasBuffer = 0,
             SaidasBuffer = 0,
             ReturnHiveBuffer = 0,   // retorna para a colmeia
             ReturnFieldBuffer = 0;  // retorna para o campo


void loop()
{
  if (Flag_Sensors == true){
    Flag_Sensors = false;
    state_Sensors = ReadingSensors();
    processaFSM(state_Sensors);
  }
 ReadButtonDisplay();// rotina para verificar se botão wake up foi pressionado 
 server.handleClient();

 if (Flag_tempo_SalvarDados == true){
  Flag_tempo_SalvarDados = false;
  SalvarDadosEmArquivo();
 }
}

// *********  Functions   *********



void SalvarDadosEmArquivo(){
  int TamanhoMaximoArquivo = 2; // Tamanho maximo para o arquivo de dados, se passar disso será criado um backup e o arquivo dados será zerado 

  String DadosAtuais,
         Entradas,
         Saidas,
         ReturnHive,   // retorna para a colmeia
         ReturnField;  // retorna para o campo

  float TamanhoArquivo;

  Serial.println("Temporizador de salvar dados no arquivo ativado\n");
  Serial.print("Intervalo entre a ultimo armazenamento: ");
  Serial.print((millis() - TempoUltimoDadoSalvo)/1000);
  Serial.println(" segundos");
  TempoUltimoDadoSalvo = millis();
  listDir(LittleFS, "/", 1); // List the directories up to one level beginning at the root directory
  // "Data, Entradas, Saidas, Bateria, Erros"
  //appendFile(LittleFS, "/Dados.csv", "Data, Entradas, Saidas, Bateria, Erros\r\n"); //Append some text to the previous file

  DadosAtuais = String(ReadtimeRTC()); // Data
  DadosAtuais +=  ",";
  DadosAtuais += String(EntradasBuffer); // Entradas

  DadosAtuais +=  ",";
  DadosAtuais += String(SaidasBuffer); // Saidas

  DadosAtuais +=  ",";
  DadosAtuais += "12.5v"; // Bateria

  DadosAtuais +=  ",";
  DadosAtuais += "nenhum\r\n"; // Erros

  //deleteFile(LittleFS, "/Backup_Dados1.csv");
  //deleteFile(LittleFS, "/Dados1.csv");

  if(!FileExiste(LittleFS, "/Dados.csv"))  // Se o arquivo Não existir, cria o arquivo e imprime o cabeçalho
  { 
    writeFile(LittleFS, "/Dados.csv", "Data, Entradas, Saidas, Bateria, Erros\r\n");
    appendFile(LittleFS, "/Dados.csv", DadosAtuais.c_str()); //Append some text to the previous file
    DeleteDadosAtuais();
  }else{
  
    TamanhoArquivo = sizeFile(LittleFS, "/Dados.csv");

    if(TamanhoArquivo > TamanhoMaximoArquivo){ // Se o arquivo Dados for maior que 1MB, cria um backup e apaga o antigo backup
     
      Serial.print("\nO arquivo Dados é Maior que ");Serial.print(TamanhoMaximoArquivo);Serial.print(" kByte \n Tamanho Arquivo Dados: ");
      Serial.print(TamanhoArquivo);
      Serial.print(" kBytes\n");

      if(!FileExiste(LittleFS, "/Backup_Dados.csv")){ // Se o backup não existir, o arquivo dados será transformado em backup
      
        renameFile(LittleFS, "/Dados.csv", "/Backup_Dados.csv"); 
        writeFile(LittleFS, "/Dados.csv", "Data, Entradas, Saidas, Bateria, Erros\r\n");
        appendFile(LittleFS, "/Dados.csv", DadosAtuais.c_str());
        DeleteDadosAtuais();
      }else{ // Se o backup existir, o backup antigo será deletado e o arquivo dados será transformado em backup
      
        deleteFile(LittleFS, "/Backup_Dados.csv");
        renameFile(LittleFS, "/Dados.csv", "/Backup_Dados.csv"); // Se o arquivo ficar maior que 1Mbyte será criado um backup (evitar ficar manipulando arquivos muito grandes)
        writeFile(LittleFS, "/Dados.csv", "Data, Entradas, Saidas, Bateria, Erros\r\n");
        appendFile(LittleFS, "/Dados.csv", DadosAtuais.c_str());
        DeleteDadosAtuais();
      }

    }else{
      Serial.print("\nO arquivo Dados é Menor que ");Serial.print(TamanhoMaximoArquivo);Serial.print(" kByte \n Tamanho Arquivo Dados: ");
      Serial.print(TamanhoArquivo);
      Serial.print(" kBytes\n");
      appendFile(LittleFS, "/Dados.csv", DadosAtuais.c_str());
      DeleteDadosAtuais();
    }
  }
}

void DeleteDadosAtuais(){
  EntradasBuffer = 0;
  SaidasBuffer = 0;

}


void ReadButtonDisplay(){
  if (digitalRead(button_WakeUp) == HIGH)
  {

    State_button_WakeUp = true;
    time_DisplayON = millis();
    Heltec.display->displayOn();
    Serial.println("");
    Serial.print(" EntradasBuffer: ");
    Serial.print(EntradasBuffer);
    Serial.println("");
    Serial.print(" SaidasBuffer: ");
    Serial.print(SaidasBuffer);
    Serial.println("");
    Serial.print(" ReturnHiveBuffer: ");
    Serial.print(ReturnHiveBuffer);
    Serial.println("");
    Serial.print(" ReturnFieldBuffer: ");
    Serial.print(ReturnFieldBuffer);
    Serial.println("");
    Serial.print(ReadBattery());
    Serial.println("");
   /* tensao = analogRead(Adc_Battery);
    Serial.print(tensao);
    Serial.println("");
    delay(500);
    */
  }

  // desliga após 30 segundos
  if (State_button_WakeUp && ((millis() - time_DisplayON) >= Time_Display)) {
    State_button_WakeUp = false;
    Heltec.display->displayOff();
  }
}

uint8_t ReadingSensors(){
  return(digitalRead(S1)<< 0) | (digitalRead(S2)<< 1) | (digitalRead(S3)<< 2);
}

unsigned int ReadtimeRTC(){
  // Retorna a data no formato timestamp Unix de  0 até 4.294.967.295 segundos
   digitalWrite(SaidaVext,LOW); 
   if (!rtc.begin(&I2C_RTC)){
     Serial.println(" RTC nao encontrado ");
     return NULL;
   }
   DateTime now = rtc.now();
   Serial.println(now.timestamp(DateTime::TIMESTAMP_FULL));
   digitalWrite(SaidaVext,HIGH); //Desliga o modulo RTC3132 para economizar consumo
   return now.unixtime();
}

void IRAM_ATTR InterruptExternSensors(){
  Flag_Sensors = true;
}

float ReadBattery(){
  // (0-3.3V) em valores digitais de 0 a 4095 (2^12)

  float tensao = 0.0, mediana;
  unsigned int Ad[10];
  char cont = 0;

  Serial.println("| ");
  for(cont=0;cont<10;cont++) {
    Ad[cont]= analogRead(Adc_Battery);
    Serial.print(Ad[cont]);
    Serial.print(" | ");
    delay(50);
  }
  Serial.print("\nMediana tensao: ");
  mediana = (Ad[4]+Ad[5]) / 2 ;
  Serial.print(mediana);
  Serial.println("");
  
  return mediana;
}










