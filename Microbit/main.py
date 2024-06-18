from flask import Flask, request, jsonify
from datetime import datetime
import sqlite3
import os

app = Flask(__name__)

db_path = './database.db'
if not os.path.exists(db_path):
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
else:
    conn = sqlite3.connect(db_path, check_same_thread=False)
    cursor = conn.cursor()

# Kontrola existence tabulky
cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='devices';")
if cursor.fetchone() is None:
    cursor.execute("CREATE TABLE devices (id INTEGER PRIMARY KEY, name TEXT, status TEXT, ip TEXT, zvukovyAlarm BOOL, svetelnyAlarm BOOL, delay TEXT);")
    cursor.execute("INSERT INTO devices (name, status, ip, zvukovyAlarm, svetelnyAlarm, delay) VALUES ('inteligentní detektor pohybu', 'OFFLINE', '192.168.212.63', TRUE, TRUE, 60);")
    cursor.execute("CREATE TABLE notices (id, device_id INTEGER, message TEXT, time TEXT);")
    conn.commit()  


@app.route('/api/user',)
def hello_world():
    print('Hello, World!')
    data = {
        "user": "Hello, World!"
    }
    return jsonify(data)

@app.route('/api/login', methods=['POST'])
def login():
    data = request.get_json()
    username = data.get('username')
    password = data.get('password')

    if username != 'admin' or password != 'admin':
        return jsonify({'message': 'Neplatné přihlašovací údaje!'}), 401
    
    else:        
        return jsonify({'message': 'Úspěšné přihlášení!'}), 200


@app.route('/api/devices', methods=['GET'])
def get_devices():
    conn = sqlite3.connect(db_path, check_same_thread=False)
    cursor = conn.cursor()


    cursor.execute("SELECT * FROM devices;")
    devices = cursor.fetchall()
    print(devices)
    return jsonify(devices)




@app.route('/api/devices/<int:id>/info', methods=['GET'])
def get_device_info(id):
    conn = sqlite3.connect(db_path, check_same_thread=False)
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM devices WHERE id = ?;", (id,))
    device = cursor.fetchone()
    print(device)
    return jsonify(device)


@app.route('/api/devices/<int:id>/notices', methods=['GET'])
def get_device_notices(id):
    conn = sqlite3.connect(db_path, check_same_thread=False)
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM notices WHERE device_id = ?;", (id,))
    notices = cursor.fetchall()
    return jsonify(notices)


@app.route('/api/devices/<id>/edit', methods=['POST'])
def update_device(id):
    try:
        data = request.get_json()
        name = data.get('name')
        zvukovyAlarm = data.get('soundAlarm')
        svetelnyAlarm = data.get('lightAlarm')
        delay = data.get('speed')

        conn = sqlite3.connect(db_path, check_same_thread=False)
        cursor = conn.cursor()

        cursor.execute("UPDATE devices SET name = ?, zvukovyAlarm = ?, svetelnyAlarm = ?, delay = ? WHERE id = ?;", (name, zvukovyAlarm, svetelnyAlarm, delay, id))
        conn.commit()
        conn.close()

        return jsonify({'message': 'Zařízení bylo úspěšně aktualizováno!'}), 200
    
    except Exception as e:
        print(e)
        return jsonify({'error': str(e)}), 400



@app.route('/api/device/online', methods=['POST'])
def device_online():
    data = request.form
    ip_address = data.get('ip')
    print(data)

    if ip_address:
        conn = sqlite3.connect(db_path, check_same_thread=False)
        cursor = conn.cursor()

        cursor.execute("UPDATE devices SET status = 'ONLINE', ip = ? WHERE id= 1;", (ip_address,))
        conn.commit()
        conn.close()
        
    else:
        return jsonify({"error": "IP address is required"}), 400
    
    return jsonify({"message": "Device status updated to ONLINE"}), 200
    




@app.route('/api/device/detection', methods=['POST'])
def detection():
    try:
        data = request.get_json()
        ip = data.get('ip')
        print(data)

        conn = sqlite3.connect(db_path, check_same_thread=False)
        cursor = conn.cursor()

        # Získání aktuálního času
        current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        cursor.execute("INSERT INTO notices (device_id, message, time) VALUES (?, ?, ?);", ("1", "Detekován pohyb", current_time))
        conn.commit()
        conn.close()

        return jsonify({'message': 'Zpráva byla úspěšně uložena!'}), 200

    except Exception as e:
        return jsonify({'error': str(e)}), 400


if __name__ == '__main__':
    conn.close()
    app.run(host='0.0.0.0', port=8080, debug=True)