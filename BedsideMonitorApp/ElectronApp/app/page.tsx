"use client"

import { useEffect, useState } from "react"
import { VitalSignsPanel } from "./components/vital-signs-panel"
import { ECGDisplay } from "./components/ecg-display"
import { RoomParameters } from "./components/room-parameters"
import { PatientInfo } from "./components/patient-info"
import { AlertPanel } from "./components/alert-panel"
import { Header } from "./components/header"

interface VitalSigns {
  heartRate: number
  bloodPressure: { systolic: number; diastolic: number }
  oxygenSaturation: number
  temperature: number
  respiratoryRate: number
  ecgValue: number
}

interface RoomParams {
  temperature: number
  humidity: number
  pressure: number
}

export default function BedsideMonitor() {
  const [vitalSigns, setVitalSigns] = useState<VitalSigns>({
    heartRate: 72,
    bloodPressure: { systolic: 120, diastolic: 80 },
    oxygenSaturation: 98,
    temperature: 37.0,
    respiratoryRate: 16,
    ecgValue: 0,
  })

  const [roomParameters, setRoomParameters] = useState<RoomParams>({
    temperature: 23,
    humidity: 50,
    pressure: 1013,
  })

  const [ecgData, setEcgData] = useState<number[]>([])

  useEffect(() => {
    // Simulate Electron IPC in browser environment
    const vitalSignsInterval = setInterval(() => {
      const newVitalSigns = {
        heartRate: 65 + Math.random() * 20,
        bloodPressure: {
          systolic: 110 + Math.random() * 20,
          diastolic: 70 + Math.random() * 15,
        },
        oxygenSaturation: 95 + Math.random() * 5,
        temperature: 36.5 + Math.random() * 1.5,
        respiratoryRate: 12 + Math.random() * 8,
        ecgValue: Math.sin(Date.now() / 100) * 50 + Math.random() * 20,
      }
      setVitalSigns(newVitalSigns)

      // Update ECG data
      setEcgData((prev) => {
        const newData = [...prev, newVitalSigns.ecgValue]
        return newData.slice(-200) // Keep last 200 points
      })
    }, 100)

    const roomInterval = setInterval(() => {
      setRoomParameters({
        temperature: 22 + Math.random() * 3,
        humidity: 45 + Math.random() * 10,
        pressure: 1013 + Math.random() * 5,
      })
    }, 5000)

    return () => {
      clearInterval(vitalSignsInterval)
      clearInterval(roomInterval)
    }
  }, [])

  return (
    <div className="min-h-screen bg-gray-900 text-white">
      <Header />

      <div className="grid grid-cols-12 gap-4 p-4 h-[calc(100vh-80px)]">
        {/* Patient Info */}
        <div className="col-span-3">
          <PatientInfo />
        </div>

        {/* Main Monitoring Area */}
        <div className="col-span-6 space-y-4">
          <ECGDisplay data={ecgData} heartRate={vitalSigns.heartRate} />
          <VitalSignsPanel vitalSigns={vitalSigns} />
        </div>

        {/* Side Panel */}
        <div className="col-span-3 space-y-4">
          <AlertPanel vitalSigns={vitalSigns} />
          <RoomParameters parameters={roomParameters} />
        </div>
      </div>
    </div>
  )
}
