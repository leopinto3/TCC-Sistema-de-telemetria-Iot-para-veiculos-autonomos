from paho.mqtt import client
import mysql.connector
from datetime import datetime
import json

conexao_mysql = mysql.connector.connect(
    host="localhost",
    user="root",
    password="senha",
    database="telemetria"
)

def inserir_info_carro(id_carro, bateria, horas_rodadas, limit_temp_bat, limitCC):
    id_carro = int(id_carro)
    bateria = int(bateria)
    horas_rodadas = float(horas_rodadas)
    limit_temp_bat = int(limit_temp_bat)
    limitCC = int(limitCC)
    cursor = conexao_mysql.cursor()
    comando_sql = """
        INSERT INTO info_carro (id_carro, bateria, horas_rodadas, limit_temp_bat, limitCC) 
        VALUES (%s, %s, %s, %s, %s)
    """
    dados = (id_carro, bateria, horas_rodadas, limit_temp_bat, limitCC)
    cursor.execute(comando_sql, dados)
    conexao_mysql.commit()
    cursor.close()

def inserir_info_rota(ID_carro, rota_IF, dh_rota, latitude, longitude):
    hora_atual = datetime.now()
    hora_formatada = hora_atual.strftime("%Y/%m/%d %H:%M:%S")
    dh_rota = datetime.strptime(hora_formatada, "%Y/%m/%d %H:%M:%S")
    cursor = conexao_mysql.cursor()
    comando_sql = """
        INSERT INTO info_rota (ID_carro, rota_IF, dh_rota, latitude, longitude) 
        VALUES (%s, %s, %s, %s, %s)
    """
    # Certifique-se de que os tipos de dados e a formatação estejam corretos
    dados = (ID_carro, rota_IF, dh_rota, latitude, longitude)
    cursor.execute(comando_sql, dados)
    conexao_mysql.commit()
    cursor.close()


def recebido(client, userdata, message):
    print('recebeu algo')
    print("Topico", message.topic, "Recebido", message.payload)

    try:
        payload_dict = json.loads(message.payload.decode())
        
        if message.topic == "dados_carro":
            inserir_info_carro(
                payload_dict.get("id_carro", 0),
                payload_dict.get("bateria", 0),
                payload_dict.get("horas_rodadas", 0),
                payload_dict.get("limit_temp_bat", 0),
                payload_dict.get("limitCC", 0)
            )
        elif message.topic == "dados_rota":
            inserir_info_rota(
                payload_dict.get("ID_carro", 1),
                payload_dict.get("rota_IF", ""),
                payload_dict.get("dh_rota", 0),
                payload_dict.get("latitude", 0),
                payload_dict.get("longitude", 0)
            )
    except json.JSONDecodeError as e:
        print(f"Erro ao decodificar JSON: {e}")

# Criar uma instância de um cliente MQTT
cliente = client.Client("cliente_ouvinte")
cliente.username_pw_set("senha", "senha")
# Conectar ao broker
cliente.connect("localhost", 1883)
print('conectado ao broker')
# Registrar a função de callback
cliente.message_callback_add("dados_carro", recebido)
cliente.message_callback_add("dados_rota", recebido)

# Inscrever o cliente nos tópicos
cliente.subscribe("dados_carro")
cliente.subscribe("dados_rota")

# Laço do cliente
cliente.loop_forever()
