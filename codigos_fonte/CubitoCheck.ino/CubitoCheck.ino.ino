/////////--------IOT--------FIAP------------///////////
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>


 
// Configurações de WiFi
const char *SSID = "claudia13";
const char *PASSWORD = "13291329";  // Substitua pelo sua senha
 
// Configurações de MQTT
const char *BROKER_MQTT = "46.17.108.113";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "fiware_smartComfort9";
const char *TOPIC_PUBLISH_ACEL = "/TEF/smartComfort9/attrs/a";
 
 
// Configurações de Hardware
 
#define PUBLISH_DELAY 2000
 
// Variáveis globais
WiFiClient espClient;
PubSubClient MQTT(espClient);
unsigned long publishUpdate = 0;
const int TAMANHO = 200;
int buzzerPin = 18;
const int endereco_MPU = 0x68;
float acel_x, acel_y, acel_z, giro_x, giro_y, giro_z, temp;

// Variáveis para monitoramento de alteração
float acel_x_anterior;
unsigned long tempo_anterior;
int contador;

 
// Protótipos de funções
void initWiFi();
void initMQTT();
void reconnectMQTT();
void reconnectWiFi();
void checkWiFIAndMQTT();


 
void initWiFi() {
  Serial.print("Conectando com a rede: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
 
  Serial.println();
  Serial.print("Conectado com sucesso: ");
  Serial.println(SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}
 
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}
 
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar com o Broker MQTT: ");
    Serial.println(BROKER_MQTT);
 
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
     } else {
      Serial.println("Falha na conexão com MQTT. Tentando novamente em 2 segundos.");
      delay(2000);
    }
  }
}
 
void checkWiFIAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) reconnectWiFi();
  if (!MQTT.connected()) reconnectMQTT();
}
 
void reconnectWiFi(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;
 
  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
 
  Serial.println();
  Serial.print("Wifi conectado com sucesso");
  Serial.print(SSID);
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}
 
void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin , OUTPUT);
  Wire.begin();
  Wire.beginTransmission(endereco_MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  initWiFi();
  initMQTT();

}
 
void loop() {



  Wire.beginTransmission(endereco_MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(endereco_MPU, 14, true);

  acel_x = Wire.read() << 8 | Wire.read();
  acel_y = Wire.read() << 8 | Wire.read();
  acel_z = Wire.read() << 8 | Wire.read();
  temp = Wire.read() << 8 | Wire.read();
  giro_x = Wire.read() << 8 | Wire.read();
  giro_y = Wire.read() << 8 | Wire.read();
  giro_z = Wire.read() << 8 | Wire.read();

  acel_x = acel_x / 16384.0;
  acel_y = acel_y / 16384.0; // fator de conversão para escala de 2g
  acel_z = acel_z / 16384.0; // fator de conversão para escala de 2g

  // Converte valor de temperatura para graus Celsius
  temp = (temp / 340.00) + 36.53; // equação do datasheet do sensor

  // Converte os valores do giroscópio lidos em velocidade (º/s)
  giro_x = (giro_x / 131.0); // fator de conversão para escala de 250º/s
  giro_y = (giro_y / 131.0); // fator de conversão para escala de 250º/s
  giro_z = (giro_z / 131.0); // fator de conversão para escala de 250º/s

  unsigned long tempo_atual = millis();

  // Verifica se houve uma mudança significativa em acel_x
  if (abs(acel_x - acel_x_anterior) > 0.6) {
    // Houve uma mudança significativa, reinicia o contador
    acel_x_anterior = acel_x;
    tempo_anterior = tempo_atual;
    // Desliga o buzzer se estiver ligado
    Serial.print("Tempo levado para efetuar a mudança de decúbito: ");
    Serial.print(contador);
    Serial.print(" segundos");
    Serial.println();
    noTone(buzzerPin);
    contador = 0;
  } else {
    // Verifica se o tempo sem mudanças é superior a 2 horas
    if(tempo_atual - tempo_anterior > 10000) {
      Serial.print("Está na hora de mudar a posição do paciente");
      Serial.println();
      // Ativa o alarme no buzzer
      tone(buzzerPin, 1000);
      // Pode adicionar aqui outras ações de alarme, como enviar uma mensagem, etc.
      contador ++;
      delay(1000);
    }
  }

  if (acel_x >= 3 && acel_x <= 4){
    acel_x = acel_x - 4;

  } else if (acel_x > 2 && acel_x <= 2.99) {
    acel_x = acel_x - 3;
  }

 
    checkWiFIAndMQTT();
  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();
    //sensor mpu6050
    char msgBuffer2[7];
    Serial.print("Posição: ");
    Serial.println(acel_x);
    dtostrf(acel_x, 4, 2, msgBuffer2);
    MQTT.publish(TOPIC_PUBLISH_ACEL,msgBuffer2);
    MQTT.loop();
 
    }


    delay(500);

  }