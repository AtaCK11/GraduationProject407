import sqlite3 as sqll3
from datetime import datetime
import ast


DB_PATH = "patient_db.db"

def get_latest_ecg_by_patient_id(patient_id):
    conn = sqll3.connect(DB_PATH)
    cursor = conn.cursor()

    try:
        # Get MAC from patient name
        cursor.execute("""
            SELECT assigneddevicemac 
            FROM patients 
            WHERE id = ?
        """, (patient_id,))
        result = cursor.fetchone()
        if not result:
            return None, f"No patient found with ID '{patient_id}'"

        mac = result[0]

        # Get latest ECG data
        cursor.execute("""
            SELECT timestamp, ecg_value 
            FROM ecg_data 
            WHERE mac = ? 
            ORDER BY timestamp DESC 
            LIMIT 1
        """, (mac,))
        ecg_record = cursor.fetchone()
        if not ecg_record:
            return None, f"No ECG data found for MAC {mac}"

        timestamp, ecg_string = ecg_record
        ecg_values = ast.literal_eval(ecg_string)  # Safely parse string list
        return ecg_values

    finally:
        conn.close()
