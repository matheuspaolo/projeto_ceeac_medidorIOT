#include <WiFiEsp.h>
// Configuração de conexão
char ssid[] = "CEEAC";
char pass[] = "c33@ufac";
// -------------------------

char server[] = "medidor.ceeac.org";
WiFiEspClient client;

void setup()
{
  Serial1.begin(115200);
  Serial.begin(115200);
  setupWiFi();
}

void loop()
{
    // Código já existente
    // Código já existente
    // ...
    // Código já existente
    
    EnviarDados(a,b,c); // 'a', 'b' e 'c' devem ser float
  
    delay(X); // X deve ter um intervalo considerável para não sobrecarregar o módulo e o servidor (X >= 5segundos pelo menos)
}

void setupWiFi()
{
  //Serial onde está o ESP-01 com o firmware AT já instalado
  WiFi.init(&Serial1);

  Serial.print("Conectando a ");
  Serial.println(ssid);

  int status = WL_IDLE_STATUS;

  for(int i = 0; i < 5; i++){
    status = WiFi.begin(ssid, pass);
    if(status == WL_CONNECTED){
      break;
      Serial.println();
      Serial.println("Conectado");
      
      IPAddress localIP = WiFi.localIP();
      Serial.print("IP: ");
      Serial.println(localIP);
    }
    
    else{
      Serial.println("Não conectado");
    }
    
    delay(1000);
}

void EnviarDados(float tensao, float corrente, float potencia){
  // Converte os argumentos de inteiros para strings (necessário para realizar o GET)
  String t = String(tensao);
  String c = String(corrente);
  String p = String(tensao*corrente);
  Serial.println("Inicializando requisição HTTP.");
  if (client.connect(server, 80)) {
    Serial.println("Conectado ao servidor.");
    String url = "/index.php/Inserir/medicao/" + t + "/" + c + "/" + p;
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: medidor.ceeac.org");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
  Serial.println("Requisição finalizada.");
}
