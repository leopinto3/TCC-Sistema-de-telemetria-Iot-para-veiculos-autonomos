import mysql.connector

# Conectando ao servidor MySQL local (certifique-se de ter o servidor MySQL em execução)
conexao = mysql.connector.connect(
    host="localhost",
    user="root",
    password="senha"  # Substitua com suas credenciais
)

# Criando o banco de dados
cursor = conexao.cursor()
cursor.execute("DROP DATABASE IF EXISTS telemetria")
cursor.execute("CREATE DATABASE IF NOT EXISTS telemetria")
cursor.execute("USE telemetria")

# Criando a tabela info_carro
cursor.execute("""
    CREATE TABLE IF NOT EXISTS info_carro (
        ID_carro INT AUTO_INCREMENT PRIMARY KEY,
        bateria INT,
        horas_rodadas INT,
        limit_temp_bat INT,
        limitCC INT
    )
""")

# Criando a tabela info_rota
cursor.execute("""
    CREATE TABLE IF NOT EXISTS info_rota (
        ID_carro INT,
        ID_rota INT AUTO_INCREMENT PRIMARY KEY,
        rota_IF VARCHAR(255),
        dh_rota VARCHAR(255),
        latitude FLOAT,
        longitude FLOAT
    )
""")

# Fechando a conexão
cursor.close()
conexao.close()
