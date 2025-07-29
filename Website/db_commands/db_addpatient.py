import sqlite3 as sqll3
from datetime import datetime

def db_add_patient(data):
    try:
        conn = sqll3.connect("patient_db.db")
        conn.row_factory = sqll3.Row
        cursor = conn.cursor()

        now = datetime.now().isoformat()

        cursor.execute('''
            INSERT INTO patients (
                assigneddevicemac,
                name,
                age,
                room,
                gender,
                disease,
                entrydate,
                assigneddoctor,
                lastupdated,
                isactive
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            data["assigneddevicemac"],
            data["name"],
            int(data["age"]),
            data["room"],
            data["gender"],
            data["disease"],
            now,           # entrydate
            data["assigneddoctor"],
            now,           # lastupdated
            1              # isactive
        ))

        conn.commit()
        conn.close()
        return True
    except Exception as e:
        print(f"Error inserting patient: {e}")
        return False
