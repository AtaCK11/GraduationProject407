import sqlite3 as sqll3
from datetime import datetime
import json

DB_PATH = "patient_db.db"
"""
CREATE TABLE IF NOT EXISTS ecg_data (
    timestamp TEXT,
    mac TEXT,
    ecg_value text,
    FOREIGN KEY (mac) REFERENCES devices(mac)
);
''')
"""

def db_add_ecg_array(ip, ecg_values):
    try:
        conn = sqll3.connect(DB_PATH)
        conn.row_factory = sqll3.Row
        cursor = conn.cursor()

        print(f"Adding ECG data for IP: {ip}, Values: {ecg_values}")

        cursor.execute("SELECT mac FROM devices WHERE ip = ?", (ip,))
        row = cursor.fetchone()

        if not row:
            print(f"No device found with IP: {ip}")
            return False
        
        mac_address = row['mac']
        timestamp = datetime.now().isoformat()
        ecg_values_json = json.dumps(ecg_values)
        cursor.execute("INSERT INTO ecg_data (mac, ecg_value, timestamp) VALUES (?, ?, ?)", (mac_address, ecg_values_json, timestamp))
        conn.commit()
        conn.close()

        return True

    except Exception as e:
        print(f"DB error: {e}")
        return False
