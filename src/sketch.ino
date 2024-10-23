// Inclusão da biblioteca para o sensor DHT (temperatura e umidade)
#include "DHTesp.h"

// Inclusão da biblioteca para o controle do display LCD via interface I2C
#include <LiquidCrystal_I2C.h>

const int dhtPin = 23;             // Pino do sensor DHT22
const int irrigationRelay = 19;    // Pino do relé de irrigação

// Inicialização do objeto para o sensor DHT
DHTesp dhtSensor;

// Inicialização do objeto LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);


 // Função que formata a escrita da temp. e umid. mediante os valores e exibi no display LCD
void displayLcd(const TempAndHumidity& data) {
  
  // Declaração das variáveis temperatura e umidade
  float temperature = data.temperature;
  float humidity = data.humidity;

  // Formatação da temperatura e umidade
  String tempString = "Temp:  " + String(temperature, 2);
  String humidityString = "Umid:  " + String(humidity, 2);

  // Ajuste de formatação para exibição no display LCD, mediante aos valores de temperatura e umidade
  if(temperature < 10 && temperature >= 0){
    tempString = "Temp:   " + String(temperature, 2);
  } 
  else if(temperature < 0 && temperature <= -10){
    tempString = "Temp: " + String(temperature, 2);
  }

  if(humidity < 10){
    humidityString = "Umid:   " + String(humidity, 2);
  }

  if(humidity >= 100){
    humidityString = "Umid: " + String(humidity, 2);
  }

  
  // Exibe temperatura e umidade no LCD
  lcd.setCursor(1, 0);
  lcd.print(tempString);

  lcd.setCursor(14, 0);
  lcd.print("C");

  lcd.setCursor(1, 1);
  lcd.print(humidityString);

  lcd.setCursor(14, 1);
  lcd.print("%");
}


// Função para atualizar temperatura e umidade caso ocorra mudança nos valores
void updatedisplayTempAndHumid(const TempAndHumidity& data) {

  // Declaração das variáveis temperatura e umidade
  float temperature = data.temperature;
  float humidity = data.humidity;

  // Inicializa com -1 para garantir a atualização na primeira leitura
  static float lastTemperature = -1;
  static float lastHumidity = -1;


  // Verifica se ouve atualização nos dados da temperatura e umidade e caso verdadeiro atualiza o display LCD
  if (temperature != lastTemperature || humidity != lastHumidity) {

    // Limpa a tela do LCD
    lcd.clear();

    // Chama a função para exibir dados no display LCD
    displayLcd(data);

    // Seta com os novos dados de temp. e umid. para nova verificação
    lastTemperature = temperature;
    lastHumidity = humidity;
  }
}

// Função para controle de irrigação
bool controlIrrigation(const TempAndHumidity& data) {
  // Declara a variável de status da irrigação
  bool statusIrrigation;

  // Declaração das variáveis temperatura e umidade
  float temperature = data.temperature;
  float humidity = data.humidity;

  // Verificação da temp. e umid. seguindo a lógica para cultivo de tomate para iniciar ou não a irrigação
  if (humidity < 65 && (temperature >= 20 && temperature <= 26)) {
    digitalWrite(irrigationRelay, HIGH);
    statusIrrigation = true; 
  } 
  else if (humidity >= 75) {
    digitalWrite(irrigationRelay, LOW);
    statusIrrigation = false; 
  } 
  else {
    digitalWrite(irrigationRelay, LOW);
    statusIrrigation = false; 
  }

  return statusIrrigation; // Retorna o status da irrigação
}

// Inicialização do código e definição das configurações que precisam ser executadas no início da programa
void setup() {
  // Inicializa a comunicação serial com velocidade de 115200 bps (bits por segundo)
  Serial.begin(115200);

  // Configura o sensor DHT (modelo DHT22) no pino especificado
  dhtSensor.setup(dhtPin, DHTesp::DHT22);

  // Define o pino do relé de irrigação como saída
  pinMode(irrigationRelay, OUTPUT);

  lcd.init(); // Inicializa o display lcd
  lcd.backlight(); // Liga a luz de fundo do display lcd
  
  // Posiciona o cursor na coluna e linha correta e exibi os textos de inicialização
  lcd.setCursor(1, 0); 
  lcd.print("Monitoramento");
  
  lcd.setCursor(2, 1);  
  lcd.print("Agricola");

  // Exibe uma animação de pontos, imprimindo um ponto a cada 500 ms
  for (int i = 11; i <= 13; i++) {
    lcd.setCursor(i, 1); 
    lcd.print(".");    
    delay(500);        
  }

  // Limpa o display LCD
  lcd.clear();
}

// Programa principal
void loop() {
  // Obtém os dados de temperatura e umidade do sensor DHT
  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  // Atualiza o display com os valores de temperatura e umidade
  updatedisplayTempAndHumid(data);
  
  // Chama a função que controla a irrigação e retorna o status
  bool statusIrrigation = controlIrrigation(data);

  // Imprime os valores de temperatura, umidade e status da irrigação no terminal serial
  Serial.println("Temp: " + String(data.temperature, 1) + 
                " C, Umid: " + String(data.humidity, 1) + 
                " %, Irrigação: " + (statusIrrigation ? "Ligado" : "Desligado"));

  // Aguarda 2 segundos antes de realizar a próxima leitura
  delay(2000);
}