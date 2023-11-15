#include <Wire.h>

const int endereco_MPU = 0x68;
float acel_x, acel_y, acel_z, giro_x, giro_y, giro_z, temp;

// Variáveis para monitoramento de alteração
float acel_x_anterior;
unsigned long tempo_anterior;
int contador;

// Pino para o buzzer
const int pinoBuzzer = 7;  // Ajuste conforme o pino utilizado

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(endereco_MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Inicializa o pino do buzzer
  pinMode(pinoBuzzer, OUTPUT);
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
    noTone(pinoBuzzer);
    contador = 0;
  } else {
    // Verifica se o tempo sem mudanças é superior a 2 horas
    if (tempo_atual - tempo_anterior > 10000) {
      Serial.print("Está na hora de mudar a posição do paciente");
      Serial.println();
      // Ativa o alarme no buzzer
      tone(pinoBuzzer, 1000);
      // Pode adicionar aqui outras ações de alarme, como enviar uma mensagem, etc.
      contador ++;
      delay(1000);
    }
  }

  // Imprime os valores no Monitor Serial (como antes)
  // Serial.print("AcX: ");
  // Serial.print(acel_x, 2);
  // ... (outros prints)
  // Serial.println();

  // Delay de 500 ms
  delay(500);
}
