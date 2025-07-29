import { AlertTriangle, CheckCircle, Clock } from "lucide-react"

interface VitalSigns {
  heartRate: number
  bloodPressure: { systolic: number; diastolic: number }
  oxygenSaturation: number
  temperature: number
  respiratoryRate: number
  ecgValue: number
}

interface AlertPanelProps {
  vitalSigns: VitalSigns
}

export function AlertPanel({ vitalSigns }: AlertPanelProps) {
  const alerts = []

  // Check for abnormal vital signs
  if (vitalSigns.heartRate < 60 || vitalSigns.heartRate > 100) {
    alerts.push({
      type: "warning",
      message: `Heart rate ${vitalSigns.heartRate < 60 ? "too low" : "too high"}: ${Math.round(vitalSigns.heartRate)} BPM`,
      time: new Date().toLocaleTimeString(),
    })
  }

  if (vitalSigns.oxygenSaturation < 95) {
    alerts.push({
      type: "critical",
      message: `Low oxygen saturation: ${Math.round(vitalSigns.oxygenSaturation)}%`,
      time: new Date().toLocaleTimeString(),
    })
  }

  if (vitalSigns.temperature < 36.1 || vitalSigns.temperature > 37.2) {
    alerts.push({
      type: "warning",
      message: `Abnormal temperature: ${vitalSigns.temperature.toFixed(1)}°C`,
      time: new Date().toLocaleTimeString(),
    })
  }

  return (
    <div className="bg-gray-800 rounded-lg p-4">
      <h3 className="text-lg font-semibold text-white mb-4">Alerts & Notifications</h3>

      <div className="space-y-3">
        {alerts.length === 0 ? (
          <div className="flex items-center space-x-3 p-3 bg-green-900/20 border border-green-500 rounded-lg">
            <CheckCircle className="h-5 w-5 text-green-400" />
            <div>
              <p className="text-green-400 font-medium">All vitals normal</p>
              <p className="text-xs text-green-300">No alerts at this time</p>
            </div>
          </div>
        ) : (
          alerts.map((alert, index) => (
            <div
              key={index}
              className={`flex items-start space-x-3 p-3 rounded-lg border ${
                alert.type === "critical" ? "bg-red-900/20 border-red-500" : "bg-yellow-900/20 border-yellow-500"
              }`}
            >
              <AlertTriangle
                className={`h-5 w-5 mt-0.5 ${alert.type === "critical" ? "text-red-400" : "text-yellow-400"}`}
              />
              <div className="flex-1">
                <p className={`font-medium ${alert.type === "critical" ? "text-red-400" : "text-yellow-400"}`}>
                  {alert.message}
                </p>
                <div className="flex items-center space-x-1 mt-1">
                  <Clock className="h-3 w-3 text-gray-400" />
                  <p className="text-xs text-gray-400">{alert.time}</p>
                </div>
              </div>
            </div>
          ))
        )}
      </div>

      <div className="mt-6 pt-4 border-t border-gray-700">
        <h4 className="text-sm font-medium text-white mb-3">Recent Activity</h4>
        <div className="space-y-2 text-xs text-gray-400">
          <p>• Vital signs monitoring started - 09:15 AM</p>
          <p>• ECG lead connected - 09:14 AM</p>
          <p>• Patient admitted to room - 09:00 AM</p>
        </div>
      </div>
    </div>
  )
}
