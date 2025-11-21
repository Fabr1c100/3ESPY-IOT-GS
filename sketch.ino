#include <WiFi.h>
#include <PubSubClient.h>

// ---- CONFIGURAÇÕES ----
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com"; 
const int mqtt_port = 1883;

// Tópicos MQTT
const char* topic_presenca  = "stepahead/device1/presenca";
const char* topic_progresso = "stepahead/device1/progresso";

// ---- PINOS ----
#define TRIG_PIN 18
#define ECHO_PIN 19
#define LED_R 12
#define LED_G 14
#define LED_B 27
#define BUZZER_PIN 23

// ---- VARIÁVEIS ----
WiFiClient espClient;
PubSubClient client(espClient);

long duration;
float distanceCm;
bool usuarioPresente = false;

unsigned long tempoInicioSessao = 0;
unsigned long tempoAcumuladoAnterior = 0;
unsigned long tempoTotalGeral = 0;
unsigned long lastLogTime = 0;

int progressoAtual = 0;          
int ultimoProgressoNotificado = -1; 

// 100 segundos = 100% (1 seg = 1%)
const int TEMPO_TOTAL_CURSO_SEG = 100; 

// --- FUNÇÕES ---

void setup_wifi() {
  Serial.print("Conectando WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println(" OK!");
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "StepAhead-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT Conectado!");
      digitalWrite(LED_B, HIGH); delay(100); digitalWrite(LED_B, LOW);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
  pinMode(LED_R, OUTPUT); pinMode(LED_G, OUTPUT); pinMode(LED_B, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(LED_R, HIGH); // Vermelho (Ausente)

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void lerSensor() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = duration * 0.034 / 2;
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  lerSensor();

  // --- LÓGICA PRINCIPAL ---
  if (distanceCm > 0 && distanceCm < 50) { 
    // --- USUÁRIO PRESENTE ---
    
    if (!usuarioPresente) {
      usuarioPresente = true;
      tempoInicioSessao = millis();
      Serial.println("\n--- INICIO DA SESSÃO ---");
      client.publish(topic_presenca, "1");
      
      // Feedback inicial
      tone(BUZZER_PIN, 2000, 100);
    }

    // CÁLCULO
    unsigned long sessaoAtualSeg = (millis() - tempoInicioSessao) / 1000;
    tempoTotalGeral = tempoAcumuladoAnterior + sessaoAtualSeg;
    progressoAtual = (tempoTotalGeral * 100) / TEMPO_TOTAL_CURSO_SEG;

    // CONTROLE VISUAL PADRÃO (VERDE ENQUANTO ESTUDA)
    digitalWrite(LED_R, LOW); 
    digitalWrite(LED_G, HIGH); // Verde Aceso (Foco)
    digitalWrite(LED_B, LOW);

    // LOG NO TERMINAL (MOSTRA SEGUNDOS RODANDO)
    if (millis() - lastLogTime > 1000) {
      lastLogTime = millis();
      Serial.print("Tempo de Foco: "); Serial.print(tempoTotalGeral);
      Serial.print("s | Progresso: "); Serial.print(progressoAtual); Serial.println("%");
      
      // Envia tempo real para MQTT (opcional, bom para grafico)
      client.publish(topic_progresso, String(progressoAtual).c_str());
    }

    // --- MOMENTO "META BATIDA" (LED AZUL) ---
    // A cada 10% (10, 20, 30...) ele muda para AZUL temporariamente
    if (progressoAtual > 0 && progressoAtual % 10 == 0 && progressoAtual != ultimoProgressoNotificado) {
      ultimoProgressoNotificado = progressoAtual;
      
      Serial.println("\n>>> META ALCANÇADA! LEVEL UP! <<<");
      Serial.print(">>> "); Serial.print(progressoAtual); Serial.println("% DO CURSO CONCLUÍDO <<<\n");
      
      // MUDA PARA AZUL (COMEMORAÇÃO)
      digitalWrite(LED_G, LOW); 
      digitalWrite(LED_B, HIGH); // AZUL ACENDE
      
      // Som de vitória
      tone(BUZZER_PIN, 1000, 150); delay(150); tone(BUZZER_PIN, 2000, 300);
      
      // Segura o azul por 2 segundos para dar tempo de ver no vídeo
      delay(2000); 
      
      // Volta a contar o tempo ajustando o delay que demos
      tempoInicioSessao += 2000; 
      
      // Volta para o VERDE (Foco)
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_G, HIGH);
    }

    // TRAVA EM 100%
    if (progressoAtual >= 100) {
       Serial.println("!!! CURSO 100% CONCLUÍDO !!!");
       // Comemoração final longa
       while(distanceCm < 50) { // Enquanto não sair da cadeira, fica piscando festa
          digitalWrite(LED_G, HIGH); digitalWrite(LED_B, LOW); delay(200);
          digitalWrite(LED_G, LOW); digitalWrite(LED_B, HIGH); delay(200);
          lerSensor();
       }
    }

  } else { 
    // --- USUÁRIO AUSENTE ---
    if (usuarioPresente) {
      usuarioPresente = false;
      unsigned long sessaoFinalizada = (millis() - tempoInicioSessao) / 1000;
      tempoAcumuladoAnterior += sessaoFinalizada;
      
      Serial.println("\n--- PAUSA DETECTADA ---");
      Serial.print("Progresso Salvo: "); Serial.print(progressoAtual); Serial.println("%");
      client.publish(topic_presenca, "0");
      tone(BUZZER_PIN, 500, 500);
    }
    
    // LED Vermelho
    digitalWrite(LED_R, HIGH); digitalWrite(LED_G, LOW); digitalWrite(LED_B, LOW);
  }
  
  delay(100);
}