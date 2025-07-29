import sqlite3 as sqll3

def db_tryregister(mac, ip, device_type="unknown"):
    try:
        conn = sqll3.connect("patient_db.db")
        cursor = conn.cursor()

        cursor.execute('''
CREATE TABLE IF NOT EXISTS devices (
mac TEXT PRIMARY KEY,
ip TEXT,
devicetype TEXT,
isassigned INTEGER
);
''')

        cursor.execute("SELECT * FROM devices WHERE mac=?", (mac,))

        existing_device = cursor.fetchone()

        if existing_device:
            print(f"Device already exists! MAC={mac}, IP={ip}")
            # update it with the new parameters
            cursor.execute("UPDATE devices SET ip=?, devicetype=?, isassigned=1 WHERE mac=?", (ip, device_type, mac))
            conn.commit()
            print(f"Device Updated! MAC={mac}, IP={ip}")
        else:
            cursor.execute("INSERT INTO devices (mac, ip, devicetype, isassigned) VALUES (?, ?, ?, ?)", (mac, ip, device_type, 1))
            conn.commit()
            print(f"Device Added! MAC={mac}, IP={ip}")

        conn.close()

        return True
    except Exception as e:
        return False