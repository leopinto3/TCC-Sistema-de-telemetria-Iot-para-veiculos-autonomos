#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "VIVOFIBRA-D8DE"; // Insira o nome da sua rede WiFi
const char* password = "72237AD8DE"; // Insira a senha da sua rede WiFi

const char* mqttServer = "192.168.15.51"; // Endereço do servidor MQTT
const int mqttPort = 1883;
const char* mqttUser = "senha"; // Usuário MQTT
const char* mqttPassword = "senha"; // Senha MQTT

WiFiClient wifiClient;
PubSubClient client(wifiClient);

String latitude[30]; // Lista para armazenar latitude
String longitude[30]; // Lista para armazenar longitude
int total_pares = 0;

bool wifiConnected = false;
bool mqttConnected = false;
bool fimReceived = false;

int id_carro = 1; // Dados fictícios
int bateria = 0;
float horas_rodadas = 0;
int limit_temp_bat = 0;
int limitCC = 0;

void connectWiFi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Conectado ao Wi-Fi");
  wifiConnected = true;
}

void connectMQTT() {
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT");
      mqttConnected = true;
    } else {
      Serial.print("Falha na conexão MQTT. Erro: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void publishCarData() {
  // Publish de dados do carro obtidos do Arduino
  String carroMsg = "{\"id_carro\":" + String(id_carro) +
                    ",\"bateria\":" + String(bateria) +
                    ",\"horas_rodadas\":" + String(horas_rodadas) +
                    ",\"limit_temp_bat\":" + String(limit_temp_bat) +
                    ",\"limitCC\":" + String(limitCC) + "}";

  if (client.publish("dados_carro", carroMsg.c_str())) {
    Serial.println("Dados do carro publicados");
  } else {
    Serial.print("Falha ao publicar dados do carro. Erro: ");
    Serial.println(client.state());
  }
}

void publishRouteData() {
  // Publicações dos pares de rota
  for (int i = 0; i < total_pares; i++) {
    String rota_IF = latitude[0] + "_ " + longitude[0] + "/" + latitude[total_pares] + "_" + longitude[total_pares];

    String rotaMsg = "{\"id_carro\":" + String(id_carro) +
                      ",\"ID_rota\":" + String(10) +
                      ",\"rota_IF\":\"" + rota_IF +
                      "\",\"dh_rota\":" + String(0) +
                      ",\"latitude\":" + latitude[i] +
                      "\"longitude\":" + longitude[i] + "}";

    if (client.publish("dados_rota", rotaMsg.c_str())) {
      Serial.print("Rota ");
      Serial.print(i + 1);
      Serial.println(" publicada");
    } else {
      Serial.print("Falha ao publicar rota ");
      Serial.print(i + 1);
      Serial.print(". Erro: ");
      Serial.println(client.state());
    }
    delay(1000);
  }

  // Define fimReceived como false para evitar repetições
  fimReceived = false;
}

void receiveEvent(int byteCount) {
  while (Wire.available()) {
    String receivedData = Wire.readStringUntil('\0'); // Lê os dados enviados pelo dispositivo Master

    if (receivedData.startsWith("fim")) {
      fimReceived = true;
      delay(1000);
      Serial.println("Recebido sinal de FIM");

      Serial.println("DADOS RECEBIDOS:");
      for (int i = 0; i < total_pares; i++) {
        Serial.print("Lat:");
        Serial.print(latitude[i]);
        Serial.print(", Long:");
        Serial.println(longitude[i]);
      }
    } else if (receivedData.startsWith("Lat:")) {
      int separator = receivedData.indexOf("Long:");
      String lat = receivedData.substring(4, separator);
      String lon = receivedData.substring(separator + 5);

      latitude[total_pares] = lat;
      longitude[total_pares] = lon;

      Serial.print("Received Lat: ");
      Serial.print(lat);
      Serial.print("Long: ");
      Serial.println(lon);

      total_pares++;
    } else if (receivedData.startsWith("bateria:")) {
        bateria = receivedData.substring(8).toInt();
        Serial.println("bateria: " + String(bateria));
    } else if (receivedData.startsWith("horas_rodadas:")) {
        horas_rodadas = receivedData.substring(14).toFloat();
        Serial.println("horas_rodadas: " + String(horas_rodadas));
    } else if (receivedData.startsWith("limit_temp_bat:")) {
        limit_temp_bat = receivedData.substring(15).toInt();
        Serial.println("limit_temp_bat: " + String(limit_temp_bat));
    } else if (receivedData.startsWith("limitCC:")) {
        limitCC = receivedData.substring(8).toInt();
        Serial.println("limitCC: " + String(limitCC));
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(8); // Inicializa o dispositivo como Slave no endereço 8
  Wire.onReceive(receiveEvent); // Define a função de recebimento

  while (!Serial) {
    delay(1000); // Aguarda a conexão serial
  }
}

void loop() {
  if (fimReceived) {
    if (!wifiConnected) {
      connectWiFi();
    }
    if (wifiConnected && !mqttConnected) {
      connectMQTT();
    }
    if (mqttConnected) {
      publishCarData();
      publishRouteData();
    }
  }
  client.loop(); // Mantém a comunicação com o servidor MQTT ativa
}
