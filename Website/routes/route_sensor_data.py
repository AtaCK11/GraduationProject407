from flask import Blueprint, request, jsonify
from db_commands.db_addsensordata import db_add_sensor_data

sensor_data_bp = Blueprint("sensor_data", __name__)

@sensor_data_bp.route('/sensor_data', methods=['POST'])
def sensor_data():
    data = request.get_json()
    if 'sensorTag' not in data:
        return jsonify({"error": "Invalid"}), 400

    sender_ip = request.remote_addr
    sensor_tag = data["sensorTag"]
    sensor_value = data["sensorValue"]

    if db_add_sensor_data(sender_ip, sensor_tag, sensor_value):
        print(f"Sensor Data Added: IP={sender_ip} TAG={sensor_tag} VALUE={sensor_value}")
        return jsonify({"status": "ok"}), 200

    return jsonify({"error": "Invalid"}), 404
