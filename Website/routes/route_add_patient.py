from flask import Blueprint, request, jsonify
from db_commands.db_addpatient import db_add_patient

add_patient_bp = Blueprint("add_patient", __name__)

@add_patient_bp.route('/api/add_patient', methods=['POST'])
def add_patient():
    data = request.get_json()

    required_fields = [
        "name", "age", "gender", "room",
        "disease", "assigneddoctor", "assigneddevicemac"
    ]
    print("Received data:", data)
    if not all(field in data for field in required_fields):
        return jsonify({"success": False, "error": "Eksik alanlar var."}), 400

    success = db_add_patient(data)
    print("ADDED data:", data)
    if success:
        return jsonify({"success": True}), 201
    else:
        return jsonify({"success": False, "error": "Veritabanı hatası"}), 500