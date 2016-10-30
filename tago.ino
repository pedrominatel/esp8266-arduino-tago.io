/* Pedro Minatel - Sistemas Embarcados
*
* Este programa envia os dados coletados do DHT11 para a plataforma de IoT
* thingspeak.com
* Data: 16 de Setembro de 2015
* Author: Pedro Minatel
* Website: http://pedrominatel.com.br
*/

//Include da lib de Wifi do ESP8266
#include <ESP8266WiFi.h>


//Definir o SSID da rede WiFi
const char* ssid = "<<<YOUR_SSID>>>";
//Definir a senha da rede WiFi
const char* password = "<<<YOUR_PASSWORD>>>";

//Colocar a API Key para escrita neste campo
//Ela é fornecida no canal que foi criado na aba API Keys
String apiKey = "<<<YOUR_TAGO_KEY>>>";
const char* server = "api.tago.io";

int time_esp = 0;

WiFiClient client;

long previousMillis = 0;        // Variável de controle do tempo
long redLedInterval = 20000;     // Tempo em ms do intervalo a ser executado
unsigned long currentMillis = 0;

void setup() {
  //Configuração da UART
  Serial.begin(115200);

  Serial.print("Conectando na rede ");
  Serial.println(ssid);

  //Inicia o WiFi
  WiFi.begin(ssid, password);

  //Loop até conectar no WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Logs na porta serial
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Conectado na rede ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void envia_dados(void){


  //Inicia um client TCP para o envio dos dados
  if (client.connect(server,80)) {

    Serial.print("CONNECTED AT TAGO\n");

    String postStr = "";

    String postData = "variable=boot&value="+String(time_esp)+"\n";

    postStr = "POST /data HTTP/1.1\n";
    postStr += "Host: api.tago.io\n";
    postStr += "Device-Token: "+apiKey+"\n";
    postStr += "_ssl: false\n";
    postStr += "Content-Type: application/x-www-form-urlencoded\n";
    postStr += "Content-Length: "+String(postData.length())+"\n";
    postStr += "\n";
    postStr += postData;

    client.print(postStr);
    //Serial.print("%d", postStr);

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

  }
  client.stop();

}

void loop() {

  currentMillis = millis();

  if (currentMillis - previousMillis > redLedInterval) {
    previousMillis = currentMillis;    // Salva o tempo atual
    //Faz a leitura do sensor
    envia_dados();
    time_esp++;
  }
}
