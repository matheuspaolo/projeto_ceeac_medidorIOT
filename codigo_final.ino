#include <LiquidCrystal.h>
#include <WiFiEsp.h>
#include "Wire.h"
#include "EmonLib.h"
#define DS1307_ADDRESS 0x68
EnergyMonitor SensorAmp;

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
int status;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //define as portas da tela LCD

// Configuracao de conexao
char ssid[] = "CEEAC";
char pass[] = "c33@ufac";
// -------------------------
char server[] = "medidor.ceeac.org";
WiFiEspClient client;

void setup() {
  SensorAmp.current(SCT, 60.606); //Calibracao do sensor de corrente
  lcd.begin(16, 2);
  Serial.begin(115200);
  Serial1.begin(115200);
  Wire.begin();

  pinMode(26, OUTPUT); //saida do rele 1
  pinMode(27, OUTPUT); //saida do rele 2
  pinMode(30, INPUT); //entrada do sensor 1
  pinMode(32, INPUT); //entrada do sensor 2
  pinMode(34, INPUT); //entrada do sensor 3
  pinMode(36, INPUT); //entrada do sensor 4
  pinMode(38, INPUT); //entrada do sensor 5
  pinMode(40, INPUT); //entrada do sensor 6
  pinMode(42, INPUT); //entrada do sensor 7

  LigarAC(true);
  LigarLampadas(true);

  setupWiFi();
}
void loop() {

  double Irms = SensorAmp.calcIrms(1480); //Calcula o valor da corrente

  estadoSensor1 = digitalRead(30);
  estadoSensor2 = digitalRead(32);
  estadoSensor3 = digitalRead(34);
  estadoSensor4 = digitalRead(36);
  estadoSensor5 = digitalRead(38);
  estadoSensor6 = digitalRead(40);
  estadoSensor7 = digitalRead(42);

  lcd.clear();
  lcd.setCursor(0, 0);

  Mostrahoras();

  if (estadoSensor1 || estadoSensor2 || estadoSensor3 || estadoSensor4 || estadoSensor5 || estadoSensor6 || estadoSensor7) {
    LigarLampadas(true);
    LigarAC(true);

    lcd.print("Humanos detectados");
    EnviarTexto("Humanos detectados");
  }

  else

  {
    lcd.print("Nenhum humano detectado");

    LigarLampadas(false);
    if (horas < 6 || horas > 21) {
      LigarAC(false);
      EnviarTexto("Ar condicionado desligado.");
    }

    EnviarTexto("Lâmpadas desligadas.");
  }

  if (minutos % 15 == 0) {
    if (status != WL_CONNECTED) {
      setupWiFi();
    }
    if (status == WL_CONNECTED) {
      EnviarDados(127, Irms, 127 * Irms); // 'a', 'b' e 'c' devem ser float
    }
  }

  t++;

  if (t == 5) {
    EnviarTexto("Sistema em funcionamento.");
    t = 0;
  }

  delay(1000);
}


// -------------------- FUNÇÕES DE ATIVAÇÃO E DESATIVAÇÃO DOS RELÉS --------------------

void LigarLampadas(bool sn) {
  if (sn) {
    digitalWrite(26, LOW);
  }
  else {
    digitalWrite(26, HIGH);
  }
}

void LigarAC(bool sn) {
  if (sn) {
    digitalWrite(27, LOW);
  }
  else {
    digitalWrite(27, HIGH);
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

void EnviarDados(float tensao, float corrente, float potencia) {
  // Converte os argumentos de inteiros para strings (necessario para realizar o GET)
  String t = String(tensao);
  String c = String(corrente);
  String p = String(tensao * corrente);
  Serial.println("Inicializando requisicao HTTP.");
  if (client.connect(server, 80)) {
    Serial.println("Conectado ao servidor.");
    String url = "/index.php/Inserir/medicao/" + t + "/" + c + "/" + p;
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: medidor.ceeac.org");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
  Serial.println("Requisicao finalizada.");
}

void EnviarTexto(String msg) {
  Serial.println("Inicializando requisicao HTTP.");
  if (client.connect(server, 80)) {
    Serial.println("Conectado ao servidor.");
    String url = "/index.php/Inserir/texto/" + msg;
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: medidor.ceeac.org");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
  Serial.println("Requisicao finalizada.");
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
