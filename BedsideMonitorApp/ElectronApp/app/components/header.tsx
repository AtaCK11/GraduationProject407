"use client"

import { Activity, Wifi, Battery, Clock } from "lucide-react"
import { useEffect, useState } from "react"

export function Header() {
  const [currentTime, setCurrentTime] = useState(new Date())

  useEffect(() => {
    const timer = setInterval(() => {
      setCurrentTime(new Date())
    }, 1000)

    return () => clearInterval(timer)
  }, [])

  return (
    <header className="bg-gray-800 border-b border-gray-700 px-6 py-4">
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-4">
          <Activity className="h-8 w-8 text-blue-400" />
          <div>
            <h1 className="text-xl font-bold text-white">Bedside Monitor</h1>
            <p className="text-sm text-gray-400">Medical Device v1.0</p>
          </div>
        </div>

        <div className="flex items-center space-x-6">
          <div className="flex items-center space-x-2">
            <Clock className="h-5 w-5 text-gray-400" />
            <span className="text-lg font-mono">{currentTime.toLocaleTimeString()}</span>
          </div>

          <div className="flex items-center space-x-2">
            <Wifi className="h-5 w-5 text-green-400" />
            <span className="text-sm text-green-400">Connected</span>
          </div>

          <div className="flex items-center space-x-2">
            <Battery className="h-5 w-5 text-green-400" />
            <span className="text-sm text-green-400">85%</span>
          </div>
        </div>
      </div>
    </header>
  )
}
