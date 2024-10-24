// Inclusão da biblioteca para o sensor DHT (temperatura e umidade)
#include "DHTesp.h"

// Inclusão da biblioteca para o controle do display LCD via interface I2C
#include <LiquidCrystal_I2C.h>

#define dhtPin 23            // Pino do sensor DHT22
#define irrigationRelay 19   // Pino do relé de irrigação                (LED AZUL ACLARO)
#define ventilationRelay 18  // Pino do relé da ventilação/Resfriamento  (LED AZUL ESCURO)
#define heatingRelay 5       // Pino do relé da aquecimento              (LED VIOLETA)
#define speakerPin 17        // Pino do buzzer para alertas

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

  /* Ativar a irrigação quando a umidade estiver abaixo de 65% e a temperatura 
  estiver dentro da faixa ideal (Entre 20 e 26 C). Desativar a irrigação quando a umidade atingir 75%.
  */

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

// Função para controle da ventilação e resfriamento
bool ventilationAndCoolingControl(const TempAndHumidity& data) {
  // Declara a variável de status da ventilação
  bool ventilationStatus;

  // Declaração das variáveis temperatura e umidade
  float humidity = data.humidity;
  float temperature = data.temperature;

  // Ativa a ventilação se a umidade for maior que 75% ou se a temperatura for maior que 26ºC
  if (humidity > 75 || temperature > 26) {
    digitalWrite(ventilationRelay, HIGH);
    ventilationStatus = true;

  // Desativa a ventilação se a umidade estiver entre 65-75% e a temperatura for menor que 26ºC
  } else if (humidity >= 65 && humidity <= 75 && temperature < 26) {
    digitalWrite(ventilationRelay, LOW);
    ventilationStatus = false;
  }

  return ventilationStatus;
}

// Função para controle de aquecimento
bool heatingControl(const TempAndHumidity& data) {
  // Declara a variável de status do aquecimento
  bool heatingStatus;

  // Declaração da variável temperatura
  float temperature = data.temperature;

  // Temperatura menor que 20ºC, ativar o relé de aquecimento
  if (temperature < 20) {
    digitalWrite(heatingRelay, HIGH);
    heatingStatus = true;

  } else {
    digitalWrite(heatingRelay, LOW);
    heatingStatus = false;
  }

  return heatingStatus;
}

// Função para gerar um alerta sonoro em casos de estados críticos
bool alertCriticalConditions(const TempAndHumidity& data){
  // Declara a variável de status para alerta crítico de condições de temperatura e umidade
  bool statusAlertCriticalConditions;

  // Declaração das variáveis temperatura e umidade
  float humidity = data.humidity;
  float temperature = data.temperature;

  // Verifica se a temperatura está fora do intervalo seguro ou se a umidade está fora do intervalo seguro
  if(temperature < 12 || temperature > 35 || humidity < 65 || humidity > 75){
    
    // Gera um alerta sonoro
    for(int i = 0; i < 2; i++) {
      tone(speakerPin, 500, 150); 
      delay(150);
      noTone(speakerPin); 
      delay(200);
    }

    statusAlertCriticalConditions = true;
  
  } else {
    noTone(speakerPin); // Desativa o som caso as condições estejam dentro dos parâmetros
    statusAlertCriticalConditions = false;
  }

  return statusAlertCriticalConditions;
}


// Inicialização do código e definição das configurações que precisam ser executadas no início da programa
void setup() {
  // Inicializa a comunicação serial com velocidade de 115200 bps (bits por segundo)
  Serial.begin(115200);

  // Configura o sensor DHT (modelo DHT22) no pino especificado
  dhtSensor.setup(dhtPin, DHTesp::DHT22);

  // Define os pino dos relés
  pinMode(irrigationRelay, OUTPUT);   // Irrigação
  pinMode(ventilationRelay, OUTPUT);  // Ventilação e resfriamento
  pinMode(heatingRelay, OUTPUT);      // Aquecimento

  // Define o pino do Buzzer como saída
  pinMode(speakerPin, OUTPUT);

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

  // Chama a função de controle da ventilação/resfriamento e retorna o status
  bool ventilationAndCoolingStatus = ventilationAndCoolingControl(data);

  // Chama a função para controle de aquecimento e retorna o status
  bool heatingStatus = heatingControl(data);

  // Chama a função de alerta para condições de temperatura e umidade críticas
  bool statusAlertCriticalConditions = alertCriticalConditions(data);

  // Imprime os valores de temperatura, umidade e status da irrigação no terminal serial
  Serial.println("Temperatura: " + String(data.temperature, 1) + 
                " C, Umidade: " + String(data.humidity, 1) + 
                " %, Irrigação: " + (statusIrrigation ? "Ligado" : "Desligado") +
                ", Ventilação/Resfriamento: " + (ventilationAndCoolingStatus ? "Ligado" : "Desligado") +
                ", Aquecimento: " + (heatingStatus ? "Ligado" : "Desligado") +
                ", Alerta Estado Crítico: " + (statusAlertCriticalConditions ? "Ligado" : "Desligado")
                );

  // Aguarda 1 segundo se alerta crítico estiver ativo ou 2 segundos caso não
  if (statusAlertCriticalConditions){
    delay(1000);

  }else{
    delay(2000);
  }
}