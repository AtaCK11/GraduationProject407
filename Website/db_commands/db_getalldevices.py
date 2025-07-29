import sqlite3 as sqll3

def db_get_all_devices():
    try:
        conn = sqll3.connect("patient_db.db")
        conn.row_factory = sqll3.Row
        cursor = conn.cursor()

        cursor.execute("SELECT * FROM devices")
        devices = cursor.fetchall()

        conn.close()

        if not devices:
            return []

        return [dict(device) for device in devices]
    except Exception as e:
        print(f"Error retrieving devices: {e}")
        return []