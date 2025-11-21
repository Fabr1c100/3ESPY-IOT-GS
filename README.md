# 🚀 StepAhead - SkillTracker - Global Solution 2025

> **O Futuro do Trabalho:** Gamificação e monitoramento de produtividade via IoT para requalificação profissional.

---

## 👨‍💻 Integrantes do Grupo - 3ESPY
* **Açussena Mautone** - RM: 552568
* **Fabrício Saavedra** - RM: 97631
* **Guilherme Akio** - RM: 98582
  
---

## 🎬 Demonstração do Projeto
Simulação Wokwi: **[[LINK DO PROJETO WOKWI](https://wokwi.com/projects/448250014033126401)]** 

👉 **[INSIRA O LINK DO PROJETO WOKWI AQUI]**

---

## 📝 Descrição do Projeto

O **StepAhead - SkillTracker** é uma solução IoT desenvolvida para enfrentar um dos maiores desafios do futuro do trabalho: a **disciplina na requalificação profissional**.

Em um mundo onde novas profissões surgem diariamente, o aprendizado contínuo é essencial. Nossa solução conecta o mundo físico ao digital, utilizando sensores para monitorar a dedicação do aluno nos estudos e recompensá-lo em tempo real, integrando esses dados à plataforma de carreira **StepAhead**.

### 🎯 O Problema
A alta taxa de desistência em cursos online e a dificuldade de manter o foco em ambientes remotos ou híbridos.

### 💡 A Solução
Um dispositivo de mesa inteligente que:
1.  **Detecta Presença:** Monitora se o aluno está fisicamente no posto de estudo.
2.  **Gamifica o Tempo:** Transforma minutos de foco em progresso (%) e XP.
3.  **Gera Recompensas Visuais:** Feedback imediato via LEDs e Som ao atingir metas.
4.  **Persistência de Dados:** Salva o progresso caso o aluno faça uma pausa, permitindo retomar de onde parou.

---

## 🛠️ Hardware e Tecnologias

O projeto foi simulado no ambiente **Wokwi** utilizando a seguinte arquitetura:

* **Microcontrolador:** ESP32 (DevKit V1)
* **Sensor de Distância:** Ultrassônico HC-SR04 (Detecção de presença < 50cm)
* **Feedback Visual:** LED RGB (Cátodo Comum)
* **Feedback Sonoro:** Buzzer (Piezo)
* **Protocolo de Comunicação:** MQTT (Broker HiveMQ)
* **Linguagem:** C++ (Arduino IDE)

---

## 🔌 Pinagem (Conexões)

| Componente | Pino ESP32 | Função |
| :--- | :--- | :--- |
| **HC-SR04 Trigger** | GPIO 18 | Disparo do pulso ultrassônico |
| **HC-SR04 Echo** | GPIO 19 | Leitura do retorno |
| **LED Red (R)** | GPIO 12 | Indicador de Ausência (Pausa) |
| **LED Green (G)** | GPIO 14 | Indicador de Foco (Estudando) |
| **LED Blue (B)** | GPIO 27 | Indicador de Meta Atingida (Level Up) |
| **Buzzer** | GPIO 23 | Feedback sonoro |

---

## ☁️ Configuração MQTT

O dispositivo se conecta a um Broker MQTT público para enviar os dados de telemetria para a nuvem (Backend do App StepAhead).

* **Broker:** `broker.hivemq.com`
* **Porta:** `1883`

### Tópicos Utilizados

1.  **`stepahead/device1/presenca`**
    * Envia `1` quando o aluno senta.
    * Envia `0` quando o aluno levanta.
2.  **`stepahead/device1/progresso`**
    * Envia a porcentagem acumulada do curso (ex: `10%`, `50%`, `100%`).

---

## 🎮 Lógica de Funcionamento (Gamificação)

A lógica do firmware foi desenhada para incentivar o usuário:

1.  **Estado: Ausente (LED Vermelho 🔴)**
    * Sistema em standby. Se houver progresso anterior, ele é salvo na memória.
2.  **Estado: Foco (LED Verde 🟢)**
    * Ao detectar o usuário (< 50cm), inicia/retoma a contagem.
    * O LED Verde permanece aceso indicando "Sessão Produtiva".
3.  **Estado: Level Up (LED Azul 🔵)**
    * A cada **10%** de progresso no curso (simulado a cada 10s), o sistema emite um som de vitória e acende o LED Azul por 2 segundos.
    * Isso gera uma micro-recompensa dopaminérgica para o aluno.
4.  **Estado: Conclusão 🏆**
    * Ao atingir 100%, o sistema toca uma música de vitória e trava o LED em Verde, indicando a finalização da meta diária.

---

## 🚀 Como Executar (Simulação)

1.  Acesse o link do projeto no **Wokwi** (link na seção de demonstração).
2.  Certifique-se de que as seguintes bibliotecas estão instaladas no `libraries.txt`:
    * `PubSubClient`
    * `ESP32Servo` 
3.  Clique no botão **Play** (Verde).
4.  Clique sobre o sensor **HC-SR04** e mova o slider para simular a presença (perto) ou ausência (longe) do aluno.
5.  Acompanhe o log no **Serial Monitor** e os dados chegando no Broker MQTT.

---

## 📋 Dependências

* [PubSubClient](https://github.com/knolleary/pubsubclient) - Para comunicação MQTT.
* WiFi.h - Biblioteca nativa do ESP32.

---
