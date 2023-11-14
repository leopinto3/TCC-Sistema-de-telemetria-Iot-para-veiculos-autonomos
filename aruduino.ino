#include <Wire.h>

bool transmitido = false;

template <typename T>
void enviarDadoI2C(const char *chave, T valor) {
  Wire.beginTransmission(8);
  Wire.write(chave);
  Wire.write(":");
  Wire.write(String(valor).c_str());
  Wire.endTransmission();

  Serial.print("Enviado para ESP32: ");
  Serial.print(chave);
  Serial.print(": ");
  Serial.println(valor);
}

void enviarDadoI2C(const char *dados, size_t tamanho) {
  Wire.beginTransmission(8);
  Wire.write(dados, tamanho);
  Wire.endTransmission();

  Serial.print("Enviado para ESP32: ");
  Serial.println(dados);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  if (!transmitido) {
    int total_pares = random(15, 31);

    // Envia os dados fictícios para o ESP32
    enviarDadoI2C("id_carro", random(100, 200));
    enviarDadoI2C("bateria", random(50, 100));
    enviarDadoI2C("horas_rodadas", random(5, 20) + random(0, 100) / 100.0);
    enviarDadoI2C("limit_temp_bat", random(80, 120));
    enviarDadoI2C("limitCC", random(40, 60));

    for (int i = 0; i < total_pares; i++) {
      float latitude = random(-90, 90) + random(0, 100) / 100.0;
      float longitude = random(-180, 180) + random(0, 100) / 100.0;

      String mensagem = "Lat:" + String(latitude, 6) + ",Long:" + String(longitude, 6);

      Serial.print("Enviado para ESP32: ");
      Serial.println(mensagem);

      enviarDadoI2C(mensagem.c_str(), mensagem.length());
      delay(500);
    }

    enviarDadoI2C("fim"); // Envia sinal de fim

    transmitido = true;
  }
}

}