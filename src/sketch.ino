// Inclusão da biblioteca para o sensor DHT (temperatura e umidade)
#include "DHTesp.h"

// Inclusão da biblioteca para o controle do display LCD via interface I2C
#include <LiquidCrystal_I2C.h>

const int dhtPin = 23;             // Pino do sensor DHT22
const int irrigationRelay = 19;    // Pino do relé de irrigação

// Inicialização do objeto para o sensor DHT
DHTesp dhtSensor;

// Inicialização do objeto LCD
LiquidCrystal_I2C LCD(0x27, 16, 2);

// Função que formata a escrita da temp. e umid. mediante os valores e exibi no display LCD
void displayLcd(const TempAndHumidity& data) {

  // Formatação da temperatura e umidade
  String tempString = "Temp.: " + String(data.temperature, 2) + " C ";
  String humidityString = "Umid.: " + String(data.humidity, 2) + " % ";

  // Ajuste de formatação para alinhamento mediante mudança dos valores de temp. e umid.
  tempString = (data.temperature < -10 || data.humidity >= 100) ? 
               "Temp.:" + String(data.temperature, 2) + " C " : 
               (data.temperature < 10) ? 
               "Temp.:   " + String(data.temperature, 2) + " C " : 
               tempString;

  humidityString = (data.humidity < 10) ? 
                   "Umid.:  " + String(data.humidity, 2) + " % " : 
                   humidityString;

  // Exibe temperatura e umidade no LCD
  LCD.setCursor(0, 0);
  LCD.print(tempString);

  LCD.setCursor(0, 1);
  LCD.print(humidityString);
}

// Função para atualizar temperatura e umidade caso ocorra mudança nos valores
void updatedisplayTempAndHumid(const TempAndHumidity& data) {

  // Inicializa com -1 para garantir a atualização na primeira leitura
  static float lastTemperature = -1;
  static float lastHumidity = -1;


  // Verifica se ouve atualização nos dados da temperatura e umidade e atualiza o display LCD
  if (data.temperature != lastTemperature || data.humidity != lastHumidity) {

    // Limpa a tela do LCD e exibi os dados no display LCD
    LCD.clear();
    displayLcd(data);

    // Seta com os novos dados de temp. e umid. para nova verificação
    lastTemperature = data.temperature;
    lastHumidity = data.humidity;
  }
}

// Função para controle de irrigação
String controlIrrigation(const TempAndHumidity& data) {
  // Declara a variável de status da irrigação
  String statusIrrigation;


  // Verificação da temp. e umid. seguindo a lógica para cultivo de tomate para iniciar ou não a irrigação
  if (data.humidity < 65 && (data.temperature >= 20 && data.temperature <= 26)) {
    digitalWrite(irrigationRelay, HIGH);
    statusIrrigation = "Ligado"; 
  } 
  else if (data.humidity >= 75) {
    digitalWrite(irrigationRelay, LOW);
    statusIrrigation = "Desligado"; 
  } 
  else {
    digitalWrite(irrigationRelay, LOW);
    statusIrrigation = "Desligado"; 
  }

  return statusIrrigation; // Retorne o status
}

// Inicialização do código e definição das configurações que precisam ser executadas no início da programa
void setup() {
  // Inicializa a comunicação serial com velocidade de 115200 bps (bits por segundo)
  Serial.begin(115200);

  // Configura o sensor DHT (modelo DHT22) no pino especificado
  dhtSensor.setup(dhtPin, DHTesp::DHT22);

  // Define o pino do relé de irrigação como saída
  pinMode(irrigationRelay, OUTPUT);


  LCD.init(); // Inicializa o display LCD
  LCD.backlight(); // Liga a luz de fundo do display LCD
  

  // Posiciona o cursor na coluna e linha correta e exibi os textos de inicialização
  LCD.setCursor(1, 0); 
  LCD.print("Monitoramento");
  
  LCD.setCursor(2, 1);  
  LCD.print("Agricola");

  // Exibe uma animação de pontos, imprimindo um ponto a cada 500 ms
  for (int i = 11; i <= 13; i++) {
    LCD.setCursor(i, 1); 
    LCD.print(".");    
    delay(500);        
  }

  // Limpa o display LCD
  LCD.clear();
}

// Programa principal
void loop() {
  // Obtém os dados de temperatura e umidade do sensor DHT
  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  // Atualiza o display com os valores de temperatura e umidade
  updatedisplayTempAndHumid(data);
  
  // Chama a função que controla a irrigação e retorna o status como String
  String statusIrrigation = controlIrrigation(data);

  // Imprime os valores de temperatura, umidade e status da irrigação no terminal serial
  Serial.println("Temp.: " + String(data.temperature, 1) + 
                 " C, Umid.: " + String(data.humidity, 1) + 
                 " %, Irrigação: " + statusIrrigation);
  
  // Aguarda 2 segundos antes de realizar a próxima leitura
  delay(2000);
}

