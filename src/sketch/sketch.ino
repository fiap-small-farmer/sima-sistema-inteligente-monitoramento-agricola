// Inclusão da biblioteca para o sensor DHT (temperatura e umidade)
#include "DHTesp.h"

// Inclusão da biblioteca para o controle do display LCD via interface I2C
#include <LiquidCrystal_I2C.h>

#define dhtPin 23                   // Pino do sensor DHT22
#define irrigationPwmPin 19         // Pino do motor para controle PWM de irrigação (LED LARANJA)
#define ventilationRelay 18         // Pino do relé da ventilação/Resfriamento      (LED AZUL)
#define heatingRelay 5              // Pino do relé da aquecimento                  (LED ROXO)
#define speakerPin 17               // Pino do buzzer para alertas
#define statusLedPinOk 13           // Pino do led verde
#define acceptableStatusLedPin 12   // Pino do led amarelo
#define criticalStatusLedPin 14     // Pino do led vermelho
#define pinWaterLevelTrigger 2      // Pino Trigger nível água
#define echoWaterLevelPin 15        // Pino Echo nível água
#define PirPresenceSensorPin 4      // Pino sensor de presença PIR
#define ldrPin 27                   // Pino sensor LDR

// Inicialização do objeto para o sensor DHT
DHTesp dhtSensor;

// Inicialização do objeto LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variável para armazenar o estado da irrigação
bool irrigationActive = true;

// Declara a variável de status para alerta crítico de condições de temperatura e umidade
bool statusAlertCriticalConditions = false;

// declara a variável para detecção de movimento
int motionDetected = false;

// Variável para declarar a condição climatica medida pelo LDR
String climaticConditions;

// Função que formata a escrita da temp. e umid. mediante os valores e exibi no display LCD
void displayLcd(const TempAndHumidity& data) {
  // Declaração das variáveis temperatura e umidade
  float temperature = data.temperature;
  float humidity = data.humidity;

  // Formatação da temperatura e umidade
  String tempString = "Temp:  " + String(temperature, 2);
  String humidityString = "Umid:  " + String(humidity, 2);

  // Ajuste de formatação para exibição no display LCD, mediante aos valores de temperatura e umidade
  if (temperature < 10 && temperature >= 0) {
    tempString = "Temp:   " + String(temperature, 2);
  }
  else if (temperature < 0 && temperature <= -10) {
    tempString = "Temp: " + String(temperature, 2);
  }

  if (humidity < 10) {
    humidityString = "Umid:   " + String(humidity, 2);
  }

  if (humidity >= 100) {
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

// Função para controle do fluxo de água do sistema de irrigação com PWM e Sensor de Luminosidade LDR
float controlIrrigationPWM(const TempAndHumidity& data, float waterLevelPercentage, int ldrPinValue) {
    // Declaração das variáveis temperatura e umidade
    float temperature = data.temperature;
    float humidity = data.humidity;

    // Declaração variável do fluxo de água em %
    float waterFlowInPercentage = 0;

    // Controle da irrigação baseado no nível de água
    if (waterLevelPercentage <= 20) {
        irrigationActive = false;  // Desliga a irrigação

        // Gera um alerta sonoro para nível do tanque baixo
        for (int i = 0; i < 4; i++) {
            tone(speakerPin, 226.2, 150);
            delay(150);
            noTone(speakerPin);
            delay(200);
        }
    } else if (waterLevelPercentage >= 50) {
        irrigationActive = true;   // Religa a irrigação
    }

    // Apenas realiza o controle do fluxo de água se a irrigação estiver ativa
    if (irrigationActive) {
        // Lógica para desligar a irrigação com base em condições de clima inadequadas
        if (temperature < 20 || temperature > 26 || humidity > 75) {
            waterFlowInPercentage = 0;  // Desativa irrigação (condições inadequadas)
        }
        // Fluxo proporcional para umidade entre 0% e 65% com temperatura ideal
        else if (humidity <= 65) {
            waterFlowInPercentage = map(humidity, 0, 65, 100, 50);  // Mapeia fluxo de 100% a 50%
        }
        // Fluxo proporcional para umidade entre >65% e <=75% com temperatura ideal
        else if (humidity > 65 && humidity <= 75) {
            waterFlowInPercentage = map(humidity, 66, 75, 49, 0);  // Mapeia fluxo de <50% a 0%
        }

        // Lê o valor do LDR no pino analógico
        int ldrValue = analogRead(ldrPinValue);

        // Conversão manual de LDR para lux com escala inversa
        float luxValue;
        if (ldrValue <= 4063) {
            // Mapear inversamente, 32 a 4063 LDR corresponde a 100000 a 0.1 lux
            luxValue = map(ldrValue, 32, 4063, 100000, 0.1); 
        } else {
            luxValue = 0.1;  // Valores acima do intervalo podem ser tratados como iluminação mínima
        }

        // Ajuste adicional com base na luminosidade (LDR)
        float lightAdjustmentFactor;

        if (luxValue < 5000) {
            lightAdjustmentFactor = 1.5; // Dias nublados: baixa luminosidade
            climaticConditions = "Nublado";

        } else if (luxValue >= 5000 && luxValue <= 99999) {
            lightAdjustmentFactor = map(luxValue, 5000, 99999, 1.5, 0.5);
            climaticConditions = "Parcialmente Nublado";

        } else {
            lightAdjustmentFactor = 0.5; // Luz excessiva: acima de 99999 lux (dias ensolarados)
            climaticConditions = "Ensolarado";
        }

        // Aplica o fator de ajuste de luz ao fluxo de água calculado
        waterFlowInPercentage *= lightAdjustmentFactor;

        // Limita o valor do fluxo de água para entre 0 e 100%
        waterFlowInPercentage = constrain(waterFlowInPercentage, 0, 100);
    } else {
        waterFlowInPercentage = 0;  // Se a irrigação está desligada, fluxo de água é zero
        noTone(speakerPin); // Desativa o som de alerta
    }

    // Ajuste do PWM da bomba (somente se água está no nível adequado)
    analogWrite(irrigationPwmPin, map(waterFlowInPercentage, 0, 100, 0, 255)); 

    return waterFlowInPercentage;
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
  } else {
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
bool alertCriticalConditions(const TempAndHumidity& data) {
  // Declaração das variáveis temperatura e umidade
  float humidity = data.humidity;
  float temperature = data.temperature;

  // Verifica se a temperatura está fora do intervalo seguro ou se a umidade está fora do intervalo seguro
  if (temperature < 12 || temperature > 35 || humidity < 60 || humidity > 80) {

    // Gera um alerta sonoro
    for (int i = 0; i < 2; i++) {
      tone(speakerPin, 226.2, 150);
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

// Função que analisa a temperatura e umidade e retorna o status geral de monitoramento
String statusIndication(const TempAndHumidity& data) {
  // Declara a variável de status geral
  String generalStatus;

  // Declaração das variáveis temperatura e umidade
  float humidity = data.humidity;
  float temperature = data.temperature;

  // Condição para status "Crítico"
  if (statusAlertCriticalConditions == true) {
    digitalWrite(statusLedPinOk, LOW); // LED Verde apagado
    digitalWrite(acceptableStatusLedPin, LOW); // LED Amarelo apagado
    digitalWrite(criticalStatusLedPin, HIGH); // LED Vermelho aceso
    generalStatus = "Crítico";
  }
  // Condição para status "OK"
  else if (temperature >= 20 && temperature <= 26 && humidity >= 65 && humidity <= 75 && irrigationActive == true) {
    digitalWrite(statusLedPinOk, HIGH); // LED Verde aceso
    digitalWrite(acceptableStatusLedPin, LOW); // LED Amarelo apagado
    digitalWrite(criticalStatusLedPin, LOW); // LED Vermelho apagado
    generalStatus = "OK";
  }
  // Condição para status "Aceitável"
  else if (((temperature >= 12 && temperature < 20) || (temperature > 26 && temperature <= 35) ||
            (humidity >= 60 && humidity < 65) || (humidity > 75 && humidity <= 80)) && irrigationActive == true) {
    digitalWrite(statusLedPinOk, LOW); // LED Verde apagado
    digitalWrite(acceptableStatusLedPin, HIGH); // LED Amarelo aceso
    digitalWrite(criticalStatusLedPin, LOW); // LED Vermelho apagado
    generalStatus = "Aceitável";
  }
  // Condição para status "Crítico" (caso não cubra os anteriores)
  else {
    digitalWrite(statusLedPinOk, LOW); // LED Verde apagado
    digitalWrite(acceptableStatusLedPin, LOW); // LED Amarelo apagado
    digitalWrite(criticalStatusLedPin, HIGH); // LED Vermelho aceso
    generalStatus = "Crítico"; // Pode ajustar a mensagem se necessário
  }

  return generalStatus;
}

// Função para medir a distância usando o HC-SR04
float measureDistance() {
    // Envia um pulso ultrassônico
    digitalWrite(pinWaterLevelTrigger, LOW);
    delayMicroseconds(2);
    digitalWrite(pinWaterLevelTrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinWaterLevelTrigger, LOW);

    // Calcula o tempo que o eco leva para retornar
    long duration = pulseIn(echoWaterLevelPin, HIGH);

    // Calcula a distância em metros (velocidade do som é 343 m/s)
    float distance = duration * 0.000343 / 2;  // Dividido por 2 para ir e voltar

    return distance;  // Retorna a distância em metros
}

// Função que converte a altura do nível de água em porcentagem
float getWaterLevelPercentage(float distanceToWater, float tankHeight) {
    // Verifica se a distância medida pelo sensor é válida
    if (distanceToWater > tankHeight) {
        return 0; // Tanque vazio
    }

    // Calcula o nível de água como porcentagem
    float waterLevel = (tankHeight - distanceToWater) / tankHeight * 100;
    return waterLevel;
}

// Função para Detectar Movimento de pessoas ou animais na plantação de tomates
void detectionOfMovementOfPeopleOrAnimals(int motionDetected){
  // Se ocorrer detecção de movimento emiti o alerta sonoro
  if (motionDetected == HIGH) {
    // Gera um alerta sonoro em forma de sirene
    for (int i = 0; i < 3; i++) {
      for (int freq = 200; freq < 600; freq += 20) {
        tone(speakerPin, freq);
        delay(25); 
      }
      
      for (int freq = 600; freq > 200; freq -= 20) {
        tone(speakerPin, freq);
        delay(25);
      }

      noTone(speakerPin);
      delay(50);
    }

  } else {
    // Desabilita alerta sonoro
    noTone(speakerPin);
  }
}

// Inicialização do código e definição das configurações que precisam ser executadas no início da programa
void setup() {
  // Inicializa a comunicação serial com velocidade de 115200 bps (bits por segundo)
  Serial.begin(115200);

  // Configura o sensor DHT (modelo DHT22) no pino especificado
  dhtSensor.setup(dhtPin, DHTesp::DHT22);

  // Define o pino para gerar sinal PWM para controle de irrigação
  pinMode(irrigationPwmPin, OUTPUT);   // Irrigação
  
  // Define os pino dos relés
  pinMode(ventilationRelay, OUTPUT);   // Ventilação e resfriamento
  pinMode(heatingRelay, OUTPUT);       // Aquecimento

  // Define o pino do Buzzer como saída
  pinMode(speakerPin, OUTPUT);

  // Define os pinos de Led do status geral como saída
  pinMode(statusLedPinOk, OUTPUT);
  pinMode(acceptableStatusLedPin, OUTPUT);
  pinMode(criticalStatusLedPin, OUTPUT);

  // Define os pinos do sensor HC-SR04 para medir o nível de água
  pinMode(pinWaterLevelTrigger, OUTPUT);
  pinMode(echoWaterLevelPin, INPUT);

  // Define o pino do sensor PIR (Presença) para detecção de movimento como entrada de sinal
  pinMode(PirPresenceSensorPin, INPUT);

  // Define o pino do sensor LDR para detecção de Luminosidade
  pinMode(ldrPin, INPUT);


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
    delay(250);
  }

  // Limpa o display LCD
  lcd.clear();
}

// Programa principal
void loop() {
  // Obtém os dados de temperatura e umidade do sensor DHT
  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  // Verifica se há erro no sensor
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Falha na leitura do sensor DHT!");

    // Exibe falha no display LCD
    lcd.setCursor(2, 0);
    lcd.print("Falha Sensor");
    lcd.setCursor(2, 1);
    lcd.print("DHT22");
    return;
  }

  // Atualiza o display com os valores de temperatura e umidade
  updatedisplayTempAndHumid(data);

  // Chama a função que captura a distância do sensor que monitora o nível de água no tanque de irrigação
  float distanceToWater = measureDistance();

  // converte a distância medida do sensor até a água em porcentagem em cm
  float tankHeight = 4; // Altura do tanque
  float waterLevelPercentage = getWaterLevelPercentage(distanceToWater, tankHeight);

  // Chama a função que controla a irrigação e retorna o status
  float statusIrrigation = controlIrrigationPWM(data, waterLevelPercentage, ldrPin);

  // Chama a função de controle da ventilação/resfriamento e retorna o status
  bool ventilationAndCoolingStatus = ventilationAndCoolingControl(data);

  // Chama a função para controle de aquecimento e retorna o status
  bool heatingStatus = heatingControl(data);

  // Chama a função de alerta para condições de temperatura e umidade críticas
  bool statusAlertCriticalConditions = alertCriticalConditions(data);

  // Chama a função que analisa a temperatura e umidade e retorna o status geral de monitoramento
  String generalStatus = statusIndication(data);

  // Chama a função para Detectar Movimento de pessoas ou animais na plantação de tomates
  detectionOfMovementOfPeopleOrAnimals(motionDetected);

  // Lê o valor do sensor PIR
  motionDetected = digitalRead(PirPresenceSensorPin);
  
  // Define a variável para detecção de movimento para ser usado no log do monitor serial
  bool detectedMovement = motionDetected == HIGH ? true : false;

  // Imprime os valores de monitoramento
  Serial.println(" ");
  Serial.println("Status Geral: " +  (statusAlertCriticalConditions ? "Temperatura ou Umidade prejudicial" : generalStatus) + "\n" +
                 "Temperatura: " + String(data.temperature, 1) + "ºC\n" +
                 "Umidade: " + String(data.humidity, 1) + "%\n" +
                 "Nível Tanque de Água Irrigação: " + String(waterLevelPercentage, 0) + "%\n" +
                 "Irrigação: " + (statusIrrigation > 0 ? "Ligado" : "Desligado") + "\n" +
                 "Fluxo de Irrigação: " + (irrigationActive ? String(statusIrrigation) + "%" : "Nível de água baixo") + "\n" +
                 "Ventilação/Resfriamento: " + (ventilationAndCoolingStatus ? "Ligado" : "Desligado") + "\n" +
                 "Aquecimento: " + (heatingStatus ? "Ligado" : "Desligado") + "\n" +
                 "Alerta Estado Crítico: " + (statusAlertCriticalConditions || !irrigationActive ? "Ligado" : "Desligado") + "\n" +
                 "Detecção de Movimento: " + (detectedMovement ? "Ativado" : "Desativado") + "\n" +
                 "Condições Climáticas: " + climaticConditions + "\n"
                );
                
  // Delay para próximo monitoramento
  statusAlertCriticalConditions ? delay(1000) : delay(2000);
}