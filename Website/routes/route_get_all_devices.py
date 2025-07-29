from flask import Blueprint, request, jsonify

get_all_devices_bp = Blueprint("get_devices_all", __name__)

@get_all_devices_bp.route('/api/get_devices_all', methods=['GET'])
def get_active_patients():
    try:
        # return all devices
        from db_commands.db_getalldevices import db_get_all_devices
        devices = db_get_all_devices()

        if not devices:
            return jsonify({"success": False, "error": "No devices found"}), 404
        
        return jsonify({"success": True, "devices": devices}), 200
    
    except Exception as e:
        print(f"Error retrieving devices: {e}")
        return jsonify({"success": False, "error": "Database error"}), 500
