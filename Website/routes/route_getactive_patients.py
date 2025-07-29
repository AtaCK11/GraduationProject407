from flask import Blueprint, request, jsonify

get_active_patients_bp = Blueprint("get_patients_all", __name__)

@get_active_patients_bp.route('/api/get_patients_all', methods=['GET'])
def get_active_patients():
    try:
        from db_commands.db_getactivepatients import db_get_active_patients
        patients = db_get_active_patients()
        
        if not patients:
            return jsonify({"success": False, "error": "No active patients found"}), 404
        
        return jsonify({"success": True, "patients": patients}), 200
    except Exception as e:
        print(f"Error retrieving active patients: {e}")
        return jsonify({"success": False, "error": "Database error"}), 500
