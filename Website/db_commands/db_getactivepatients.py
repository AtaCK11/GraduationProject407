import sqlite3 as sqll3
from datetime import datetime

def db_get_active_patients():
    try:
        conn = sqll3.connect("patient_db.db")
        conn.row_factory = sqll3.Row
        cursor = conn.cursor()

        cursor.execute('''
            SELECT * FROM patients WHERE isactive = 1
        ''')
        patients = cursor.fetchall()

        conn.close()
        
        if not patients:
            return None
        
        return [dict(patient) for patient in patients]
    except Exception as e:
        print(f"Error retrieving active patients: {e}")
        return None