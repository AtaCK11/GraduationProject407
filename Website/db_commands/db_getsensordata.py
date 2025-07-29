import sqlite3 as sqll3
from datetime import datetime
import ast

DB_PATH = "patient_db.db"

SENSOR_TAGS = {
    1: "BODY_ECG",
    2: "BODY_HR",
    3: "BODY_BO2",
    4: "ROOM_TEMP",
    5: "ROOM_HUM",
    6: "BODY_TEMP",
    7: "ROOM_CO",
    8: "ROOM_ALCOHOL",
    9: "ROOM_CO2",
    10: "ROOM_TOULENE",
    11: "ROOM_NH4",
    12: "ROOM_ACETONE"
}

def get_latest_sensor_data(mac):
    conn = sqll3.connect(DB_PATH)
    cursor = conn.cursor()
    result = {}

    try:
        for tag_id, tag_name in SENSOR_TAGS.items():
            cursor.execute("""
                SELECT timestamp, sensor_value
                FROM sensor_data
                WHERE mac = ? AND sensor_tag = ?
                ORDER BY timestamp DESC
                LIMIT 1
            """, (mac, tag_id))
            row = cursor.fetchone()
            if row:
                timestamp, value = row
                result[tag_name] = {
                    "timestamp": timestamp,
                    "value": value
                }
            else:
                result[tag_name] = None
    finally:
        conn.close()

    return result