from flask import Blueprint, jsonify
import sqlite3 as sqll3
from db_commands.db_getsensordata import get_latest_sensor_data

get_sensor_data_bp = Blueprint("get_sensor_data", __name__)

DB_PATH = "patient_db.db"

@get_sensor_data_bp.route("/get_sensor_data/<int:patient_id>", methods=["GET"])
def api_sensor_data(patient_id):
    if not isinstance(patient_id, int) or patient_id <= 0:
        return jsonify({"error": "Invalid patient ID"}), 400

    conn = sqll3.connect(DB_PATH)
    cursor = conn.cursor()

    try:
        cursor.execute("""
            SELECT assigneddevicemac
            FROM patients
            WHERE id = ?
        """, (patient_id,))
        row = cursor.fetchone()
        if not row:
            return jsonify({"error": f"No patient found with id '{patient_id}'"}), 404

        mac = row[0]
        sensor_data = get_latest_sensor_data(mac)
        return jsonify({
            "patient": patient_id,
            "mac": mac,
            "sensors": sensor_data
        })

    finally:
        conn.close()