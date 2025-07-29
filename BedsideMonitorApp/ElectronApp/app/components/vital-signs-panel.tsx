import { Heart, Thermometer, Wind, Droplets } from "lucide-react"

interface VitalSigns {
  heartRate: number
  bloodPressure: { systolic: number; diastolic: number }
  oxygenSaturation: number
  temperature: number
  respiratoryRate: number
  ecgValue: number
}

interface VitalSignsPanelProps {
  vitalSigns: VitalSigns
}

export function VitalSignsPanel({ vitalSigns }: VitalSignsPanelProps) {
  const getStatusColor = (value: number, normal: [number, number]) => {
    if (value < normal[0] || value > normal[1]) {
      return "text-red-400 bg-red-900/20 border-red-500"
    }
    return "text-green-400 bg-green-900/20 border-green-500"
  }

  return (
    <div className="grid grid-cols-2 gap-4">
      {/* Heart Rate */}
      <div className={`p-4 rounded-lg border-2 ${getStatusColor(vitalSigns.heartRate, [60, 100])}`}>
        <div className="flex items-center space-x-3">
          <Heart className="h-8 w-8" />
          <div>
            <p className="text-sm font-medium">Heart Rate</p>
            <p className="text-2xl font-bold">{Math.round(vitalSigns.heartRate)}</p>
            <p className="text-xs opacity-75">BPM</p>
          </div>
        </div>
      </div>

      {/* Blood Pressure */}
      <div className={`p-4 rounded-lg border-2 ${getStatusColor(vitalSigns.bloodPressure.systolic, [90, 140])}`}>
        <div className="flex items-center space-x-3">
          <Droplets className="h-8 w-8" />
          <div>
            <p className="text-sm font-medium">Blood Pressure</p>
            <p className="text-2xl font-bold">
              {Math.round(vitalSigns.bloodPressure.systolic)}/{Math.round(vitalSigns.bloodPressure.diastolic)}
            </p>
            <p className="text-xs opacity-75">mmHg</p>
          </div>
        </div>
      </div>

      {/* Oxygen Saturation */}
      <div className={`p-4 rounded-lg border-2 ${getStatusColor(vitalSigns.oxygenSaturation, [95, 100])}`}>
        <div className="flex items-center space-x-3">
          <Wind className="h-8 w-8" />
          <div>
            <p className="text-sm font-medium">SpO₂</p>
            <p className="text-2xl font-bold">{Math.round(vitalSigns.oxygenSaturation)}</p>
            <p className="text-xs opacity-75">%</p>
          </div>
        </div>
      </div>

      {/* Temperature */}
      <div className={`p-4 rounded-lg border-2 ${getStatusColor(vitalSigns.temperature, [36.1, 37.2])}`}>
        <div className="flex items-center space-x-3">
          <Thermometer className="h-8 w-8" />
          <div>
            <p className="text-sm font-medium">Temperature</p>
            <p className="text-2xl font-bold">{vitalSigns.temperature.toFixed(1)}</p>
            <p className="text-xs opacity-75">°C</p>
          </div>
        </div>
      </div>
    </div>
  )
}
