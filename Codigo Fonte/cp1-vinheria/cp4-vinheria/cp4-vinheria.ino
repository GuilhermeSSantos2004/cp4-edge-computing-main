
/*  Library -----------------------------------------------------------*/
#include <math.h>  // math Library
#include "Arduino.h"
#include "WiFi.h"             // WiFi Library 
#include <HTTPClient.h>

/* Definiçoes de controle -----------------------------------------------------------*/

#define R0 10000 // Resistência do LDR em condições de iluminação de referência
float A = 1.00;   // Fator de correção de interceptação
float B_VALUE = -0.50; // Fator de correção de inclinação

/* Controle de Luz -----------------------------------------------------------*/
/* Controle de temperatura -----------------------------------------------------------*/
/* Controle de umidade -----------------------------------------------------------*/

/* Mapeamento dos pinos -----------------------------------------------------------*/

float r; //resistencia
const int ldrPin = 34;  /* Sensor de luminosidade Pino D34 do ESP*/  
const int sensorUmidPin  = 2;  /* Sensor de umidade Pino D14 do ESP */
const int sensorTempPin  = 2;   /* Sensor de temperatura  Pino D2 do ESP */




// Fatores de calibração (ajuste esses valores com base na calibração)
const float luxPorVolts = 200;  // Fator de conversão de volts para lux 

/* Definiçoes de Variaveis -----------------------------------------------------------*/

int valorLDR = 0;
float temperatura = 0;
float umidade = 0;

/* Configuração do Wi-Fi ---------------------------------------------------------*/


char wifiSsid[] = "-----"; // Nome da rede Wi-Fi
char wifiPass[] = "-----";     // Senha da rede Wi-Fi


char serverAddress[] = "https://api.tago.io/data";  // endereço TagoIO 
char contentHeader[] = "application/json";
char tokenHeader[]   = "-----"; // TagoIO Token



HTTPClient client;

void setup() {
  Serial.begin(9600); // Configurando a comunicação serial do console
  pinMode(ldrPin, INPUT);
  init_wifi(); //função para conectar no host
  pinMode (sensorTempPin, INPUT);
  pinMode (sensorUmidPin, INPUT);
}

/**

   @breve função de loop infinito

*/

void loop() {

  send_humidity(); //coleta dados da temperatura local
  send_temperature(); //coleta dados da umidade local
  send_lux(); // Para enviar dados de luminosidade
  delay(5000); //atraso de 5 segundos para inicia sequência de coleta
}

/**

   @Inicializa a conexão wifi

*/

void init_wifi(void) {

  Serial.println("Conectando Wifi...");

  Serial.print("SSID: ");

  Serial.println(wifiSsid);

  Serial.print("PASS: ");

  Serial.println(wifiPass);

  WiFi.begin(wifiSsid, wifiPass);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  }

  Serial.println("WiFi Conectado!");

  Serial.print("IP is ");

  Serial.println(WiFi.localIP());

}



/**

   @coleta dodos da temperatura

*/

void send_temperature(void) {

  // Le o valor da temperatura
  int valorSensorTemp = analogRead(sensorTempPin);

  // Converte o valor lido para tensão (0-3.3V no ESP32)
  float tensao_temp = (valorSensorTemp * 3.3) / 4095;  // Use 4095 para ESP32 (12 bits de resolução)

  // Agora, você precisa de informações sobre como a tensão se relaciona com a temperatura
  // Suponha que a relação seja linear:
  // Tensão de 0V = 0°C, Tensão de 3.3V = 100°C (apenas um exemplo)

  float temperatura = (tensao_temp - 0.0) * (100.0 / 3.3);  // Ajuste os valores conforme suas especificações

  Serial.print("Temperatura: ");

  Serial.print("Temperatura: ");
  Serial.println(temperatura);

  char anyData[30];

  char postData[300];

  char anyData1[30];

  char bAny[30];

  int statusCode = 0;



  strcpy(postData, "{\n\t\"variable\": \"temperatura\",\n\t\"value\": ");

  dtostrf(temperatura, 6, 2, anyData);

  strncat(postData, anyData, 100);

  strcpy(anyData1, ",\n\t\"unit\": \"ºC\"\n\t}\n");

  strncat (postData, anyData1, 100);

  Serial.println(postData);

  client.begin(serverAddress);

  client.addHeader("Content-Type", contentHeader);

  client.addHeader("Device-Token", tokenHeader);

  statusCode = client.POST(postData);



  // read the status code and body of the response

  Serial.print("Status code: ");

  Serial.println(statusCode);

  Serial.println("End of POST to TagoIO");

  Serial.println();

}


/**

   @coleta dodos da umidade

*/

void send_humidity(void) {


  // Le o valor de Umidade
  int valorSensorUmi = analogRead(sensorUmidPin);

  //transforma o valor do potenciometro em uma escala até 100
  umidade = map(valorSensorUmi, 0, 1023, 0, 100);

  Serial.print("Umidade: ");
  Serial.println(umidade);


  char anyData[30];

  char postData[300];

  char anyData1[30];

  char bAny[30];

  int statusCode = 0;

  strcpy(postData, "{\n\t\"variable\": \"umidade\",\n\t\"value\": ");

  dtostrf(umidade, 6, 2, anyData);

  strncat(postData, anyData, 100);

  strcpy(anyData1, ",\n\t\"unit\": \"%\"\n\t}\n");

  strncat (postData, anyData1, 100);

  Serial.println(postData);

  client.begin(serverAddress);

  client.addHeader("Content-Type", contentHeader);

  client.addHeader("Device-Token", tokenHeader);

  statusCode = client.POST(postData);


  // read the status code and body of the response

  Serial.print("Status code: ");

  Serial.println(statusCode);

  Serial.println("End of POST to TagoIO");

  Serial.println();

}


/**

   @coleta dodos da luminosidade

*/

void send_lux(void) {



  char anyData[30];

  char postData[300];

  char anyData1[30];

  char bAny[30];

  int statusCode = 0;

  int LDR_value = analogRead(ldrPin);  // Faça a leitura em bits (leitura de 12 bits)
  float voltage_ldr = LDR_value * (5.0 / 4095);  // Converte bits em volts (5V no ESP32)
  float lux = voltage_ldr * luxPorVolts;  // Converte volts em lux

  Serial.println("----------------------------------");
  Serial.print("Valor em bits: ");
  Serial.println(LDR_value);
  Serial.print("Valor em volts: ");
  Serial.println(voltage_ldr, 2);  // Exibe o valor com 2 casas decimais
  Serial.print("Iluminância (lux): ");
  Serial.println(lux, 2);  // Exibe o valor de lux com 2 casas decimais





  strcpy(postData, "{\n\t\"variable\": \"lux\",\n\t\"value\": ");

  dtostrf(valorLDR, 6, 2, anyData);

  strncat(postData, anyData, 100);

  strcpy(anyData1, ",\n\t\"unit\": \"Lux\"\n\t}\n");

  strncat (postData, anyData1, 100);

  Serial.println(postData);

  client.begin(serverAddress);

  client.addHeader("Content-Type", contentHeader);

  client.addHeader("Device-Token", tokenHeader);

  statusCode = client.POST(postData);


  Serial.print("Status code: ");

  Serial.println(statusCode);

  Serial.println("End of POST to TagoIO");

  Serial.println();



}
