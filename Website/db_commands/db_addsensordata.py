import sqlite3 as sqll3
from datetime import datetime

def db_add_sensor_data(ip, tag, value):
    try:
        conn = sqll3.connect("patient_db.db")
        conn.row_factory = sqll3.Row
        cursor = conn.cursor()

        cursor.execute("SELECT mac FROM devices WHERE ip = ?", (ip,))
        row = cursor.fetchone()

        if not row:
            return False

        mac_address = row['mac']
        timestamp = datetime.now().isoformat()

        conn.execute("INSERT INTO sensor_data (mac, sensor_tag, sensor_value, timestamp) VALUES (?, ?, ?, ?)", (mac_address, tag, value, timestamp,))
        conn.commit()
        conn.close()

        return True
    except Exception as e:
        print(e)
        return False