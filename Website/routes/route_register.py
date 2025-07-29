from flask import Blueprint, request, jsonify
from db_commands.db_tryregister import db_tryregister

register_bp = Blueprint("register", __name__)

@register_bp.route('/register', methods=['POST'])
def register_device():
    data = request.get_json()
    if 'mac' not in data:
        return jsonify({"error": "Invalid"}), 400

    mac = data["mac"]
    ip = data["ip"]
    device_type = data.get("deviceType", "unknown")

    if db_tryregister(mac, ip, device_type):
        print(f"Registered Client: MAC={mac} , IP={ip}")
        return jsonify({"status": "ok"}), 200

    return jsonify({"error": "Invalid"}), 400

