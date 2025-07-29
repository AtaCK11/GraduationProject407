import sqlite3 as sqll3

def db_deactivate_patient(patient_id):
    try:
        conn = sqll3.connect("patient_db.db")
        conn.row_factory = sqll3.Row
        cursor = conn.cursor()

        # Get assigned device MAC from the patient
        cursor.execute("SELECT assigneddevicemac FROM patients WHERE id = ?", (patient_id,))
        row = cursor.fetchone()

        if not row:
            conn.close()
            return False, "Patient not found"

        mac = row["assigneddevicemac"]

        # Set patient as inactive
        cursor.execute("UPDATE patients SET isactive = 0 WHERE id = ?", (patient_id,))

        # Unassign device
        cursor.execute("UPDATE devices SET isassigned = 0 WHERE mac = ?", (mac,))

        conn.commit()
        conn.close()
        return True, None
    except Exception as e:
        print(f"Error deactivating patient: {e}")
        return False, "Database error"
