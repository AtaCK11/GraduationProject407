from flask import Blueprint, request, jsonify
from db_commands.db_getecgdata import get_latest_ecg_by_patient_id

get_ecg_bp = Blueprint("get_ecg", __name__)

@get_ecg_bp.route('/get_ecg/<int:patient_id>', methods=['GET'])
def get_ecg_of_patient(patient_id):
    """
    Endpoint to get ECG data of a patient by their ID.
    """
    if not isinstance(patient_id, int) or patient_id <= 0:
        return jsonify({"error": "Invalid patient ID"}), 400

    try:
        ecg_data = get_latest_ecg_by_patient_id(patient_id)
        #print(ecg_data)
        if ecg_data is None:
            return jsonify({"error": "Patient not found"}), 404
        
        return jsonify({"ecgData": ecg_data}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500
