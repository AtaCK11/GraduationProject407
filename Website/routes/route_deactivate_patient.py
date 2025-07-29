from flask import Blueprint, request, jsonify
from db_commands.db_deactivatepatient import db_deactivate_patient

deactivate_patient_bp = Blueprint("deactivate_patient", __name__)

@deactivate_patient_bp.route('/api/patients/<int:patient_id>', methods=['DELETE'])
def delete_patient(patient_id):
    success, error_message = db_deactivate_patient(patient_id)

    if success:
        return jsonify({"success": True, "message": "Hasta devre dışı bırakıldı"}), 200
    else:
        return jsonify({"success": False, "error": error_message}), 400
