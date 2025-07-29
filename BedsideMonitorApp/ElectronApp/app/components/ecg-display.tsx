"use client"

import { useEffect, useRef } from "react"

interface ECGDisplayProps {
  data: number[]
  heartRate: number
}

export function ECGDisplay({ data, heartRate }: ECGDisplayProps) {
  const canvasRef = useRef<HTMLCanvasElement>(null)

  useEffect(() => {
    const canvas = canvasRef.current
    if (!canvas) return

    const ctx = canvas.getContext("2d")
    if (!ctx) return

    // Clear canvas
    ctx.fillStyle = "#111827"
    ctx.fillRect(0, 0, canvas.width, canvas.height)

    // Draw grid
    ctx.strokeStyle = "#374151"
    ctx.lineWidth = 1

    // Vertical lines
    for (let x = 0; x < canvas.width; x += 20) {
      ctx.beginPath()
      ctx.moveTo(x, 0)
      ctx.lineTo(x, canvas.height)
      ctx.stroke()
    }

    // Horizontal lines
    for (let y = 0; y < canvas.height; y += 20) {
      ctx.beginPath()
      ctx.moveTo(0, y)
      ctx.lineTo(canvas.width, y)
      ctx.stroke()
    }

    // Draw ECG waveform
    if (data.length > 1) {
      ctx.strokeStyle = "#10b981"
      ctx.lineWidth = 2
      ctx.beginPath()

      const stepX = canvas.width / data.length
      const centerY = canvas.height / 2

      data.forEach((value, index) => {
        const x = index * stepX
        const y = centerY - value * 2

        if (index === 0) {
          ctx.moveTo(x, y)
        } else {
          ctx.lineTo(x, y)
        }
      })

      ctx.stroke()
    }
  }, [data])

  return (
    <div className="bg-gray-800 rounded-lg p-4">
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-lg font-semibold text-white">ECG - Lead II</h3>
        <div className="flex items-center space-x-4">
          <span className="text-2xl font-bold text-green-400">{Math.round(heartRate)} BPM</span>
          <div className="text-sm text-gray-400">25 mm/s, 10 mm/mV</div>
        </div>
      </div>

      <canvas
        ref={canvasRef}
        width={800}
        height={200}
        className="w-full h-48 bg-gray-900 rounded border border-gray-700"
      />
    </div>
  )
}
