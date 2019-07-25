#include <WiFiEsp.h>

char ssid[] = "Naolo pet";
char pass[] = "paolonetnet";
char server[] = "paolin.xyz";
String dado1 = "66";
String dado2 = "90";
String dado3 = "09";

//char server[] = "arduino.cc";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10000L;

WiFiEspClient client;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  //Serial onde está o ESP-01 com firmware AT
  Serial1.begin(115200);

  setupWiFi();

    Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    // Make a HTTP request


    String url = "/igniter/index.php/Cadastrar/medicao/" + dado1 + "/" + dado2 + "/" + dado3;
    // Make a HTTP request:
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: www.paolin.xyz");
    client.println("Connection: close");
    client.println();

  }

}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them


  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}

// this method makes a HTTP connection to the server




void setupWiFi()
{
  //Serial onde está o ESP-01 com o firmware AT já instalado
  WiFi.init(&Serial1);

  Serial.print("Conectando a ");
  Serial.println(ssid);

  int status = WL_IDLE_STATUS;

  //Aguarda conectar à rede WiFi
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(ssid, pass);
  }

  Serial.println();
  Serial.println("Conectado");

  //Configura o IP
  //IPAddress ipAddress;
  //ipAddress.fromString(ip);
  //WiFi.config(ipAddress);

  //Veririca o IP
  IPAddress localIP = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(localIP);
}


