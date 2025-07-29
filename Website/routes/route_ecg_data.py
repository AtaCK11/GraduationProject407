from flask import Blueprint, request, jsonify
from db_commands.db_addecgarray import db_add_ecg_array

ecg_data_bp = Blueprint("ecg_data", __name__)

@ecg_data_bp.route('/ecg_data', methods=['POST'])
def ecg_data():
    data = request.get_json()
    if not data or 'ecgData' not in data:
        return jsonify({"error": "Invalid data format"}), 400
    
    sender_ip = request.remote_addr
    ecg_data_array = data['ecgData']

    #print(f"Received ECG data: {ecg_data_array}")

    if not isinstance(ecg_data_array, list):
        return jsonify({"error": "ecgData must be a list"}), 400
    if not all(isinstance(value, (int, float)) for value in ecg_data_array):
        return jsonify({"error": "All values in ecgData must be numbers"}), 400
    if len(ecg_data_array) == 0:
        return jsonify({"error": "ecgData cannot be empty"}), 400
    try:
        db_add_ecg_array(sender_ip, ecg_data_array)
        return jsonify({"message": "ECG data added successfully"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500
