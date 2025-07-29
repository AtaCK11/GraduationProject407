
import sqlite3 as sqll3
from flask import Flask, request, jsonify
import requests
import socket
import time

from routes.route_add_patient import add_patient_bp
from routes.route_deactivate_patient import deactivate_patient_bp
from routes.route_ecg_data import ecg_data_bp
from routes.route_get_patient import get_single_patient_bp
from routes.route_getactive_patients import get_active_patients_bp
from routes.route_register import register_bp
from routes.route_sensor_data import sensor_data_bp
from routes.route_get_all_devices import get_all_devices_bp
from routes.route_get_time import get_time_bp
from routes.route_get_ecg_data import get_ecg_bp
from routes.route_get_sensor_data import get_sensor_data_bp


from flask_cors import CORS


app = Flask(__name__)

app.register_blueprint(add_patient_bp)
app.register_blueprint(deactivate_patient_bp)
app.register_blueprint(ecg_data_bp)
app.register_blueprint(get_single_patient_bp)
app.register_blueprint(get_active_patients_bp)
app.register_blueprint(register_bp)
app.register_blueprint(sensor_data_bp)
app.register_blueprint(get_all_devices_bp)
app.register_blueprint(get_time_bp)

app.register_blueprint(get_ecg_bp)
app.register_blueprint(get_sensor_data_bp)


CORS(app, origins=["http://127.0.0.1:3000"])
CORS(app, origins=["http://localhost:3000"])

conn = sqll3.connect("patient_db.db")
cursor = conn.cursor()

DEBUG = True

cursor.execute('''
CREATE TABLE IF NOT EXISTS devices (
    mac TEXT PRIMARY KEY,
    ip TEXT,
    devicetype TEXT,
    isassigned INTEGER
);
''')
cursor.execute('''
CREATE TABLE IF NOT EXISTS sensor_data (
    timestamp TEXT,
    mac TEXT,
    sensor_tag INTEGER,
    sensor_value REAL,
    FOREIGN KEY (mac) REFERENCES devices(mac)
);
''')
cursor.execute('''
CREATE TABLE IF NOT EXISTS ecg_data (
    timestamp TEXT,
    mac TEXT,
    ecg_value text,
    FOREIGN KEY (mac) REFERENCES devices(mac)
);
''')
cursor.execute('''
CREATE TABLE IF NOT EXISTS patients (
    assigneddevicemac TEXT,
    id INTEGER PRIMARY KEY,
    name TEXT,
    age INTEGER,
    room TEXT,
    gender TEXT,
    disease TEXT,
    entrydate TEXT,
    assigneddoctor TEXT,
    lastupdated TEXT,
    isactive INTEGER,
    FOREIGN KEY (assigneddevicemac) REFERENCES devices(mac)
);
''')
cursor.execute('''
CREATE TABLE IF NOT EXISTS notifications (
    assigneddevicemac TEXT,
    patientid INTEGER,
    severity INTEGER,
    alert TEXT,
    FOREIGN KEY (patientid) REFERENCES patients(id)
);
''')
cursor.execute('''
CREATE TABLE IF NOT EXISTS batch_sensor_data (
    timestamp TEXT,
    mac TEXT,
    sensor_tag INTEGER,
    sensor_values TEXT,  -- JSON encoded array of floats
    FOREIGN KEY (mac) REFERENCES devices(mac)
);
''')


conn.commit()
conn.close()

# write isactive = 0 in all devices
def reset_devices():
    conn = sqll3.connect("patient_db.db")
    cursor = conn.cursor()
    cursor.execute("UPDATE devices SET isassigned = 0")
    conn.commit()
    conn.close()

reset_devices()

if not DEBUG:
    pc_ip = socket.gethostbyname(socket.gethostname())

    esp32_ap_ip = "192.168.4.1"

    pc_db_register_url = f"http://{esp32_ap_ip}/write_pc_db_server"
    payload = {'pc_ip': pc_ip }
    pc_db_taken = False
    while not pc_db_taken:
        try:
            response = requests.post(pc_db_register_url, json=payload)
            if response.status_code == 200:
                print("PC IP registered.")
                pc_db_taken = True
            else:
                print(f"PC IP fail. Error code={response.status_code}")
        except Exception as e:
            print(f"died {e}")
        time.sleep(5)

app.run(host="0.0.0.0", port=5000)