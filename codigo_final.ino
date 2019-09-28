#include <LiquidCrystal_I2C.h>
#include <WiFiEsp.h>
#include "Wire.h"
#include "EmonLib.h"
#define DS1307_ADDRESS 0x68
EnergyMonitor SensorAmp;

String h = "35";
byte zero = 0x00;
bool estadoSensor1;
bool estadoSensor2;
bool estadoSensor3;
bool estadoSensor4;
bool estadoSensor5;
bool estadoSensor6;
bool estadoSensor7;
int segundos;
int minutos;
int horas;
int SCT = A0;
int t;
int V = 127;

int status;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Criando um LCD de 16x2 no endereço 0x20

// Configuracao de conexao
char ssid[] = "CEEAC";
char pass[] = "c33@ufac";
// -------------------------
char server[] = "medidor.ceeac.org";
WiFiEspClient client;

void setup() {
  SensorAmp.current(SCT, 60.606); //Calibracao do sensor de corrente
  lcd.init();                 // Inicializando o LCD
  lcd.backlight();            // Ligando o BackLight do LCD
  Serial.begin(115200);
  Serial1.begin(115200);
  Wire.begin();

  pinMode(30, OUTPUT); //saida do rele 1
  pinMode(31, OUTPUT); //saida do rele 2
  pinMode(8, INPUT); //entrada do sensor 1
  pinMode(9, INPUT); //entrada do sensor 2
  pinMode(10, INPUT); //entrada do sensor 3
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  LigarAC(true);
  LigarLampadas(true);

  setupWiFi();
}
void loop() {


  double Irms = SensorAmp.calcIrms(1480); //Calcula o valor da corrente

  estadoSensor1 = digitalRead(8);
  estadoSensor2 = digitalRead(9);
  estadoSensor3 = digitalRead(10);


  lcd.clear();
  lcd.setCursor(0, 0);

  if (estadoSensor1 || estadoSensor2 || estadoSensor3) {
    LigarLampadas(true);
    LigarAC(true);

    lcd.print("Humanos");
    lcd.setCursor(0, 1);
    lcd.print("detectados");
    if (t == 5) {
      EnviarTexto("Humanos_detectados");
      EnviarDados(V, Irms);
    }
  }

  else

  {
    lcd.print("Nenhum humano detectado");

    LigarLampadas(false);
    if (horas < 6 || horas > 21) {
      LigarAC(false);
      if (t == 5) {
        EnviarTexto("Ar_condicionado_desligado.");

      }
    }
    if (t == 5) {
      EnviarTexto("Nenhum_humano_detectado");
      EnviarTexto("Lampadas_desligadas");
      EnviarDados(V, Irms);
    }
  }


  // ----------- ENVIAR A CADA 15 MINUTOS ----------------
  /*if (minutos % 15 == 0) {
    if (status != WL_CONNECTED) {
      setupWiFi();
    }
    if (status == WL_CONNECTED) {
      EnviarDados(V, Irms);
    }
  }*/

  t++;

  if (t > 5) {
    t = 0;
  }

  Mostrahoras();
  Serial.println(Irms);
  delay(1000);
}


// -------------------- FUNÇÕES DE ATIVAÇÃO E DESATIVAÇÃO DOS RELÉS --------------------

void LigarLampadas(bool sn) {
  if (sn) {
    digitalWrite(31, LOW);
  }
  else {
    digitalWrite(31, HIGH);
  }
}

void LigarAC(bool sn) {
  if (sn) {
    digitalWrite(30, LOW);
  }
  else {
    digitalWrite(30, HIGH);
  }
}

// -------------------- FUNÇÕES DE COMUNICAÇÃO COM O SERVIDOR --------------------

void setupWiFi()
{
  //Serial onde esta o ESP-01 com o firmware AT ja instalado
  WiFi.init(&Serial1);

  Serial.print("Conectando a ");
  Serial.println(ssid);

  status = WL_IDLE_STATUS;

  for (int i = 0; i < 2; i++) {
    status = WiFi.begin(ssid, pass);
    if (status == WL_CONNECTED) {
      Serial.println();
      Serial.println("Conectado");

      IPAddress localIP = WiFi.localIP();
      Serial.print("IP: ");
      Serial.println(localIP);
      break;
    }
    else {
      Serial.println("Nao conectado");
    }

    delay(1000);
  }
}

void EnviarDados(float tensao, float corrente) {
  // Converte os argumentos de inteiros para strings (necessario para realizar o GET)
  String t = String(tensao);
  String c = String(corrente);
  String p = String(tensao * corrente);
  Serial.println("Inicializando requisicao para envio de valores numericos.");
  if (client.connect(server, 80)) {
    Serial.println("Conectado ao servidor.");
    String url = "/index.php/Inserir/medicao/" + t + "/" + c + "/" + p;
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: medidor.ceeac.org");
    client.println("Connection: keep-alive");
    url = "/index.php/Inserir/texto/" + h + "/";
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: medidor.ceeac.org");
    client.println();
    client.stop();


  }
  Serial.println("Requisicao finalizada.");
  Serial.println("");
}

void EnviarTexto(String numero) {
  // Converte os argumentos de inteiros para strings (necessario para realizar o GET)
  //delay(2000);
  //String mensagem = String(numero);
  Serial.println("Inicializando requisicao para envio de texto.");
  if (client.connect(server, 80)) {
    Serial.println("Conectado ao servidor.");
    String url = "/index.php/Inserir/texto/" + numero;
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: medidor.ceeac.org");
    client.println("Connection: keep-alive");
    client.println();
    client.stop();
  }
  Serial.println("Requisicao finalizada.");
  Serial.println("");
}

// -------------------- FUNÇÕES DO RTC --------------------

void Mostrahoras()
{
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  segundos = BCDtoDecimal(Wire.read());
  minutos = BCDtoDecimal(Wire.read());
  horas = BCDtoDecimal(Wire.read() & 0b111111);
}

byte BCDtoDecimal(byte var)  { //Converte de BCD para decimal
  return ( (var / 16 * 10) + (var % 16) );
}
