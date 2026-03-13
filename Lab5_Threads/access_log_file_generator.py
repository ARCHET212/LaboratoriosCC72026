import random
from datetime import datetime, timedelta

# En lugar de solo 5, generaremos un rango de IPs y URLs
def generate_random_ip():
    # Genera IPs en el rango 192.168.1.1 - 192.168.1.255
    return f"192.168.1.{random.randint(1, 255)}"

def generate_random_url():
    bases = ["/index.html", "/api/data", "/login", "/dashboard", "/search", "/admin"]
    # Agrega un ID aleatorio para que cada URL sea tratada como única por la tabla hash
    return f"{random.choice(bases)}?user_id={random.randint(1, 500)}"

METHODS = ["GET", "POST", "PUT", "DELETE"]
STATUS_CODES = [200, 403, 404, 500]
start_time = datetime(2024, 2, 10, 10, 20, 30)

# Puedes cambiar este número para crear el archivo pequeño (5000) o el grande (100000)
num_lines = 5000 
file_name = "access.log"

with open(file_name, "w") as f:
    for i in range(num_lines):
        ip = generate_random_ip()
        method = random.choice(METHODS)
        url = generate_random_url()
        status = random.choice(STATUS_CODES)

        timestamp = start_time + timedelta(seconds=i)
        timestamp_str = timestamp.strftime("%d/%b/%Y:%H:%M:%S")

        log_entry = f'{ip} - - [{timestamp_str}] "{method} {url}" {status}\n'
        f.write(log_entry)

print(f"✅ Generado '{file_name}' con {num_lines} entradas dinámicas.")