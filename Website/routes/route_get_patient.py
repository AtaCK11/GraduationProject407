from flask import Blueprint, jsonify
import sqlite3 as sqll3

get_single_patient_bp = Blueprint("patient", __name__)
DB_PATH = "patient_db.db"

@get_single_patient_bp.route('/api/get_patient/<int:patient_id>', methods=['GET'])
def get_patient(patient_id):
    try:
        conn = sqll3.connect(DB_PATH)
        conn.row_factory = sqll3.Row
        cursor = conn.cursor()

        cursor.execute("SELECT * FROM patients WHERE id = ?", (patient_id,))
        row = cursor.fetchone()
        conn.close()

        if not row:
            return jsonify({"success": False, "error": "Patient not found"}), 404

        # Convert row to dict
        patient_data = dict(row)
        return jsonify({"success": True, "patient": patient_data}), 200

    except Exception as e:
        print(f"Error retrieving patient: {e}")
        return jsonify({"success": False, "error": "Internal server error"}), 500
