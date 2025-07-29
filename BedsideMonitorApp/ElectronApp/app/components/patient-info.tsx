import { User, Calendar, MapPin, Phone } from "lucide-react"

export function PatientInfo() {
  return (
    <div className="bg-gray-800 rounded-lg p-6 h-full">
      <h3 className="text-lg font-semibold text-white mb-6">Patient Information</h3>

      <div className="space-y-6">
        <div className="flex items-center space-x-3">
          <User className="h-6 w-6 text-blue-400" />
          <div>
            <p className="text-sm text-gray-400">Patient Name</p>
            <p className="text-white font-medium">John Smith</p>
          </div>
        </div>

        <div className="flex items-center space-x-3">
          <Calendar className="h-6 w-6 text-blue-400" />
          <div>
            <p className="text-sm text-gray-400">Date of Birth</p>
            <p className="text-white font-medium">March 15, 1975</p>
          </div>
        </div>

        <div className="flex items-center space-x-3">
          <MapPin className="h-6 w-6 text-blue-400" />
          <div>
            <p className="text-sm text-gray-400">Room</p>
            <p className="text-white font-medium">ICU-204</p>
          </div>
        </div>

        <div className="flex items-center space-x-3">
          <Phone className="h-6 w-6 text-blue-400" />
          <div>
            <p className="text-sm text-gray-400">Emergency Contact</p>
            <p className="text-white font-medium">+1 (555) 123-4567</p>
          </div>
        </div>

        <div className="border-t border-gray-700 pt-4">
          <p className="text-sm text-gray-400 mb-2">Medical Record #</p>
          <p className="text-white font-mono">MR-2024-001234</p>
        </div>

        <div>
          <p className="text-sm text-gray-400 mb-2">Attending Physician</p>
          <p className="text-white font-medium">Dr. Sarah Johnson</p>
        </div>

        <div>
          <p className="text-sm text-gray-400 mb-2">Admission Date</p>
          <p className="text-white font-medium">January 15, 2024</p>
        </div>
      </div>
    </div>
  )
}
