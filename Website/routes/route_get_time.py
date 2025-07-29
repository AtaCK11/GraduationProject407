from flask import Blueprint, request, jsonify
from db_commands.db_addsensordata import db_add_sensor_data

get_time_bp = Blueprint("get_time", __name__)

@get_time_bp.route('/get_time', methods=['GET'])
def get_time():
    # Send the current server time
    """
    {
        "hour": 3,
        "minute": 19,
        "second": 35,
        "date": "2025-06-09",
        "day_name": "Monday"
    }
    """
    from datetime import datetime
    now = datetime.now()
    response = {
        "hour": now.hour,
        "minute": now.minute,
        "second": now.second,
        "date": now.strftime("%Y-%m-%d"),
        "day_name": now.strftime("%A")
    }
    return jsonify(response), 200

