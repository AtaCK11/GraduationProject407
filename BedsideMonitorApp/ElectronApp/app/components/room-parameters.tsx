import { Thermometer, Droplets, Gauge } from "lucide-react"

interface RoomParams {
  temperature: number
  humidity: number
  pressure: number
}

interface RoomParametersProps {
  parameters: RoomParams
}

export function RoomParameters({ parameters }: RoomParametersProps) {
  return (
    <div className="bg-gray-800 rounded-lg p-4">
      <h3 className="text-lg font-semibold text-white mb-4">Room Environment</h3>

      <div className="space-y-4">
        <div className="flex items-center justify-between p-3 bg-gray-700 rounded-lg">
          <div className="flex items-center space-x-3">
            <Thermometer className="h-5 w-5 text-blue-400" />
            <span className="text-white">Temperature</span>
          </div>
          <span className="text-white font-bold">{parameters.temperature.toFixed(1)}°C</span>
        </div>

        <div className="flex items-center justify-between p-3 bg-gray-700 rounded-lg">
          <div className="flex items-center space-x-3">
            <Droplets className="h-5 w-5 text-blue-400" />
            <span className="text-white">Humidity</span>
          </div>
          <span className="text-white font-bold">{Math.round(parameters.humidity)}%</span>
        </div>

        <div className="flex items-center justify-between p-3 bg-gray-700 rounded-lg">
          <div className="flex items-center space-x-3">
            <Gauge className="h-5 w-5 text-blue-400" />
            <span className="text-white">Pressure</span>
          </div>
          <span className="text-white font-bold">{Math.round(parameters.pressure)} hPa</span>
        </div>
      </div>

      <div className="mt-6 pt-4 border-t border-gray-700">
        <h4 className="text-sm font-medium text-white mb-3">Device Status</h4>
        <div className="space-y-2">
          <div className="flex items-center justify-between text-sm">
            <span className="text-gray-400">ECG Sensor</span>
            <span className="text-green-400">Connected</span>
          </div>
          <div className="flex items-center justify-between text-sm">
            <span className="text-gray-400">SpO₂ Sensor</span>
            <span className="text-green-400">Connected</span>
          </div>
          <div className="flex items-center justify-between text-sm">
            <span className="text-gray-400">Temperature Probe</span>
            <span className="text-green-400">Connected</span>
          </div>
          <div className="flex items-center justify-between text-sm">
            <span className="text-gray-400">Data Transmission</span>
            <span className="text-green-400">Active</span>
          </div>
        </div>
      </div>
    </div>
  )
}
