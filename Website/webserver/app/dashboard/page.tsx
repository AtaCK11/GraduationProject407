"use client"

import type React from "react"

import Link from "next/link"
import { useState, useEffect } from "react"
import { Activity, Bell, User, LogOut, Search, Plus, Thermometer, Heart } from "lucide-react"
import { Button } from "@/components/ui/button"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Input } from "@/components/ui/input"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs"
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog"
import { Label } from "@/components/ui/label"

// Add interface for Patient type
interface Patient {
  id: number
  assigneddevicemac: string
  name: string
  age: number
  room: string
  gender: string
  disease: string
  entrydate: string
  assigneddoctor: string
  isactive: number
  // Mock vital data fields
  heartRate?: number
  ecgStatus?: string
  bodyTemp?: string
  lastUpdate?: string
  status?: string
}

// Mock devices data
const mockDevices = [
  {
    mac: "AA:BB:CC:DD:EE:07",
    ip: "192.168.1.107",
    assignedPatient: null,
    status: "Çevrimdışı",
    lastSeen: "2 saat önce",
    batteryLevel: 45,
    firmwareVersion: "v2.1.1",
  },
  {
    mac: "A0:B7:65:28:CE:A0",
    ip: "192.168.1.108",
    assignedPatient: null,
    status: "Beklemede",
    lastSeen: "30 dk önce",
    batteryLevel: 88,
    firmwareVersion: "v2.1.3",
  }
]

// Mock notifications data
const mockNotifications = [
  
]

export default function DashboardPage() {
  const [searchTerm, setSearchTerm] = useState("")
  const [statusFilter, setStatusFilter] = useState("all")
  const [doctorFilter, setDoctorFilter] = useState("all")
  const [roomFilter, setRoomFilter] = useState("all")
  const [deviceSearchTerm, setDeviceSearchTerm] = useState("")
  const [deviceStatusFilter, setDeviceStatusFilter] = useState("all")
  const [isAddPatientOpen, setIsAddPatientOpen] = useState(false)
  const [isAddDeviceOpen, setIsAddDeviceOpen] = useState(false)

  // New patient form state
  const [newPatient, setNewPatient] = useState({
    name: "",
    age: "",
    gender: "",
    room: "",
    disease: "",
    assigneddoctor: "",
    assigneddevicemac: "",
  })

  // New device form state
  const [newDevice, setNewDevice] = useState({
    mac: "",
    ip: "",
  })

  // Update the patients state declaration
  const [patients, setPatients] = useState<Patient[]>([])
  const [isLoadingPatients, setIsLoadingPatients] = useState(true)

  // Fetch patients from API
  useEffect(() => {
    const fetchPatients = async () => {
      try {
        setIsLoadingPatients(true)
        const response = await fetch("http://localhost:5000/api/get_patients_all")
        const result = await response.json()

        if (result.success) {
          // Add mock vital data to each patient for display
          const patientsWithVitals: Patient[] = result.patients.map((patient: any) => ({
            ...patient,
            heartRate: 60 + Math.floor(Math.random() * 40), // 60-100 BPM
            ecgStatus: Math.random() > 0.8 ? "Düzensiz" : "Normal",
            bodyTemp: (36.0 + Math.random() * 1.5).toFixed(1), // 36.0-37.5°C
            lastUpdate: `${Math.floor(Math.random() * 30) + 1} dk önce`,
            status: Math.random() > 0.7 ? "Kritik" : Math.random() > 0.4 ? "İzleniyor" : "Stabil",
          }))
          setPatients(patientsWithVitals)
        } else {
          console.error("Error fetching patients:", result.error)
        }
      } catch (error) {
        console.error("Error fetching patients:", error)
      } finally {
        setIsLoadingPatients(false)
      }
    }

    fetchPatients()
  }, [])


    // Fetch patients from API
  useEffect(() => {
    const fetchDevices = async () => {
      try {
        setIsLoadingPatients(true)
        const response = await fetch("http://localhost:5000/api/get_devices_all")
        const result = await response.json()

        if (result.success) {
          // Add mock vital data to each device for display
          const devicesWithVitals = result.devices.map((device: any) => ({
            ...device,
            //lastSeen: "2 dk önce",
            batteryLevel: 100,
            firmwareVersion: "v2.1.3",
          }))
          mockDevices.push(...devicesWithVitals) // Append to mockDevices
        } else {
          console.error("Error fetching devices:", result.error)
        }
      } catch (error) {
        console.error("Error fetching devices:", error)
      } finally {
        setIsLoadingPatients(false)
      }
    }

    fetchDevices()
  }, [])
  // Filter patients based on search and filters
  const filteredPatients = patients.filter((patient) => {
    const matchesSearch = patient.name.toLowerCase().includes(searchTerm.toLowerCase())
    const matchesStatus = statusFilter === "all" || patient.status === statusFilter
    const matchesDoctor = doctorFilter === "all" || patient.assigneddoctor === doctorFilter
    const matchesRoom = roomFilter === "all" || patient.room === roomFilter

    return matchesSearch && matchesStatus && matchesDoctor && matchesRoom
  })

  // Filter devices based on search and filters
  const filteredDevices = mockDevices.filter((device) => {
    const matchesSearch =
      device.mac.toLowerCase().includes(deviceSearchTerm.toLowerCase()) ||
      (device.assignedPatient && device.assignedPatient.toLowerCase().includes(deviceSearchTerm.toLowerCase()))
    const matchesStatus = deviceStatusFilter === "all" || device.status === deviceStatusFilter

    return matchesSearch && matchesStatus
  })

  // Get unique values for filters
  const uniqueDoctors = [...new Set(patients.map((p) => p.assigneddoctor))]
  const uniqueRooms = [...new Set(patients.map((p) => p.room))]
  const uniqueStatuses = [...new Set(patients.map((p) => p.status))].filter((status): status is string => typeof status === "string")
  const uniqueDeviceStatuses = [...new Set(mockDevices.map((d) => d.status))]

  // Get available devices (not assigned to any patient)
  const availableDevices = mockDevices.filter((device) => !device.assignedPatient)

  // Handle add patient form submission
  const handleAddPatient = async (e: React.FormEvent) => {
    e.preventDefault()

    try {
      const response = await fetch("http://localhost:5000/api/add_patient", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          ...newPatient,
          age: Number.parseInt(newPatient.age),
          entrydate: new Date().toISOString().split("T")[0], // Today's date
        }),
      })

      const result = await response.json()

      if (result.success) {
        // Reset form
        setNewPatient({
          name: "",
          age: "",
          gender: "",
          room: "",
          disease: "",
          assigneddoctor: "",
          assigneddevicemac: "",
        })
        setIsAddPatientOpen(false)

        // Refresh patient list
        const response = await fetch("http://localhost:5000/api/get_patients_all")
        const updatedResult = await response.json()
        if (updatedResult.success) {
          const patientsWithVitals: Patient[] = updatedResult.patients.map((patient: any) => ({
            ...patient,
            heartRate: 60 + Math.floor(Math.random() * 40),
            ecgStatus: Math.random() > 0.8 ? "Düzensiz" : "Normal",
            bodyTemp: (36.0 + Math.random() * 1.5).toFixed(1),
            lastUpdate: `${Math.floor(Math.random() * 30) + 1} dk önce`,
            status: Math.random() > 0.7 ? "Kritik" : Math.random() > 0.4 ? "İzleniyor" : "Stabil",
          }))
          setPatients(patientsWithVitals)
        }

        alert("Hasta başarıyla eklendi!")
      } else {
        alert("Hata: " + result.error)
      }
    } catch (error) {
      alert("Bir hata oluştu: " + error)
    }
  }

  // Handle add device form submission
  const handleAddDevice = async (e: React.FormEvent) => {
    e.preventDefault()

    try {
      const response = await fetch("http://localhost:5000/api/get_devices_all", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(newDevice),
      })

      const result = await response.json()

      if (result.success) {
        // Reset form
        setNewDevice({
          mac: "",
          ip: "",
        })
        setIsAddDeviceOpen(false)
        // In a real app, you would refresh the device list here
        alert("Cihaz başarıyla eklendi!")
      } else {
        alert("Hata: " + result.error)
      }
    } catch (error) {
      alert("Bir hata oluştu: " + error)
    }
  }

  return (
    <div className="flex min-h-screen flex-col">
      <header className="sticky top-0 z-10 flex h-16 items-center gap-4 border-b bg-white px-4 dark:bg-gray-950 md:px-6">
        <Link href="/" className="flex items-center gap-2 font-semibold">
          
          <span>Microcontroller Based Smart Patient Health Monitoring</span>
        </Link>
        <nav className="ml-auto flex gap-4 sm:gap-6">
          <Link href="/">
            <Button variant="ghost" size="sm">
              <LogOut className="h-5 w-5" />
              <span className="sr-only">Çıkış</span>
            </Button>
          </Link>
        </nav>
      </header>
      <main className="flex-1 p-4 md:p-6">
        <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3 mb-6">
          <Card>
            <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
              <CardTitle className="text-sm font-medium">Toplam Hasta</CardTitle>
              <User className="h-4 w-4 text-muted-foreground" />
            </CardHeader>
            <CardContent>
              <div className="text-2xl font-bold">{patients.filter((p) => p.isactive === 1).length}</div>
            </CardContent>
          </Card>
          <Card>
            <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
              <CardTitle className="text-sm font-medium">Aktif Cihazlar</CardTitle>
              <Activity className="h-4 w-4 text-muted-foreground" />
            </CardHeader>
            <CardContent>
              <div className="text-2xl font-bold">{mockDevices.filter((d) => d.status === "Çevrimiçi").length}</div>
            </CardContent>
          </Card>
          <Card>
            <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
              <CardTitle className="text-sm font-medium">Kritik Uyarılar</CardTitle>
              <Bell className="h-4 w-4 text-muted-foreground" />
            </CardHeader>
            <CardContent>
              <div className="text-2xl font-bold">
                {mockNotifications.filter((n) => n.severity === "Kritik").length}
              </div>
            </CardContent>
          </Card>
        </div>

        <Tabs defaultValue="patients">
          <TabsList>
            <TabsTrigger value="patients">Hastalar</TabsTrigger>
            <TabsTrigger value="devices">Cihazlar</TabsTrigger>
            <TabsTrigger value="notifications">Uyarılar</TabsTrigger>
          </TabsList>

          {/* Hastalar Sekmesi */}
          <TabsContent value="patients" className="mt-4">
            <Card>
              <CardHeader>
                <div className="flex flex-col md:flex-row justify-between items-start md:items-center gap-4">
                  <div>
                    <CardTitle>Hasta Listesi</CardTitle>
                    <CardDescription>Tüm hastaların gerçek zamanlı vital bulguları</CardDescription>
                  </div>
                  <Dialog open={isAddPatientOpen} onOpenChange={setIsAddPatientOpen}>
                    <DialogTrigger asChild>
                      <Button size="sm">
                        <Plus className="h-4 w-4 mr-2" />
                        Yeni Hasta Ekle
                      </Button>
                    </DialogTrigger>
                    <DialogContent className="sm:max-w-[425px]">
                      <DialogHeader>
                        <DialogTitle>Yeni Hasta Ekle</DialogTitle>
                        <DialogDescription>Yeni hasta bilgilerini girin ve bir cihaz atayın.</DialogDescription>
                      </DialogHeader>
                      <form onSubmit={handleAddPatient}>
                        <div className="grid gap-4 py-4">
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="name" className="text-right">
                              Ad Soyad
                            </Label>
                            <Input
                              id="name"
                              value={newPatient.name}
                              onChange={(e) => setNewPatient({ ...newPatient, name: e.target.value })}
                              className="col-span-3"
                              required
                            />
                          </div>
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="age" className="text-right">
                              Yaş
                            </Label>
                            <Input
                              id="age"
                              type="number"
                              value={newPatient.age}
                              onChange={(e) => setNewPatient({ ...newPatient, age: e.target.value })}
                              className="col-span-3"
                              required
                            />
                          </div>
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="gender" className="text-right">
                              Cinsiyet
                            </Label>
                            <Select
                              value={newPatient.gender}
                              onValueChange={(value) => setNewPatient({ ...newPatient, gender: value })}
                            >
                              <SelectTrigger className="col-span-3">
                                <SelectValue placeholder="Cinsiyet seçin" />
                              </SelectTrigger>
                              <SelectContent>
                                <SelectItem value="Erkek">Erkek</SelectItem>
                                <SelectItem value="Kadın">Kadın</SelectItem>
                              </SelectContent>
                            </Select>
                          </div>
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="room" className="text-right">
                              Oda
                            </Label>
                            <Input
                              id="room"
                              value={newPatient.room}
                              onChange={(e) => setNewPatient({ ...newPatient, room: e.target.value })}
                              className="col-span-3"
                              required
                            />
                          </div>
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="disease" className="text-right">
                              Hastalık
                            </Label>
                            <Input
                              id="disease"
                              value={newPatient.disease}
                              onChange={(e) => setNewPatient({ ...newPatient, disease: e.target.value })}
                              className="col-span-3"
                              required
                            />
                          </div>
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="doctor" className="text-right">
                              Doktor
                            </Label>
                            <Select
                              value={newPatient.assigneddoctor}
                              onValueChange={(value) => setNewPatient({ ...newPatient, assigneddoctor: value })}
                            >
                              <SelectTrigger className="col-span-3">
                                <SelectValue placeholder="Doktor seçin" />
                              </SelectTrigger>
                              <SelectContent>
                                {uniqueDoctors.map((doctor) => (
                                  <SelectItem key={doctor} value={doctor}>
                                    {doctor}
                                  </SelectItem>
                                ))}
                                <SelectItem value="Dr. Doktor">Dr. Örnek Örnek</SelectItem>
                              </SelectContent>
                            </Select>
                          </div>
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="device" className="text-right">
                              Cihaz
                            </Label>
                            <Select
                              value={newPatient.assigneddevicemac}
                              onValueChange={(value) => setNewPatient({ ...newPatient, assigneddevicemac: value })}
                            >
                              <SelectTrigger className="col-span-3">
                                <SelectValue placeholder="Cihaz seçin" />
                              </SelectTrigger>
                              <SelectContent>
                                {availableDevices.map((device) => (
                                  <SelectItem key={device.mac} value={device.mac}>
                                    {device.mac} ({device.ip})
                                  </SelectItem>
                                ))}
                              </SelectContent>
                            </Select>
                          </div>
                        </div>
                        <DialogFooter>
                          <Button type="button" variant="outline" onClick={() => setIsAddPatientOpen(false)}>
                            İptal
                          </Button>
                          <Button type="submit">Hasta Ekle</Button>
                        </DialogFooter>
                      </form>
                    </DialogContent>
                  </Dialog>
                </div>

                {/* Search and Filters */}
                <div className="flex flex-col md:flex-row gap-4 mt-4">
                  <div className="relative flex-1">
                    <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-4 w-4 text-gray-400" />
                    <Input
                      placeholder="Hasta adına göre ara..."
                      value={searchTerm}
                      onChange={(e) => setSearchTerm(e.target.value)}
                      className="pl-10"
                    />
                  </div>
                  <div className="flex gap-2">
                    <Select value={doctorFilter} onValueChange={setDoctorFilter}>
                      <SelectTrigger className="w-[160px]">
                        <SelectValue placeholder="Doktor" />
                      </SelectTrigger>
                      <SelectContent>
                        <SelectItem value="all">Tüm Doktorlar</SelectItem>
                        {uniqueDoctors.map((doctor) => (
                          <SelectItem key={doctor} value={doctor}>
                            {doctor}
                          </SelectItem>
                        ))}
                      </SelectContent>
                    </Select>

                    <Select value={roomFilter} onValueChange={setRoomFilter}>
                      <SelectTrigger className="w-[120px]">
                        <SelectValue placeholder="Oda" />
                      </SelectTrigger>
                      <SelectContent>
                        <SelectItem value="all">Tüm Odalar</SelectItem>
                        {uniqueRooms.map((room) => (
                          <SelectItem key={room} value={room}>
                            Oda {room}
                          </SelectItem>
                        ))}
                      </SelectContent>
                    </Select>
                  </div>
                </div>
              </CardHeader>
              <CardContent>
                {isLoadingPatients ? (
                  <div className="text-center py-8">
                    <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-primary mx-auto"></div>
                    <p className="mt-2 text-gray-500">Hastalar yükleniyor...</p>
                  </div>
                ) : filteredPatients.length === 0 ? (
                  <div className="text-center py-8 text-gray-500">Arama kriterlerinize uygun hasta bulunamadı.</div>
                ) : (
                  <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
                    {filteredPatients.map((patient, index) => (
                      <Card key={patient.assigneddevicemac} className="hover:shadow-lg transition-shadow">
                        <CardHeader className="pb-3">
                          <div className="flex justify-between items-start">
                            <div>
                              <CardTitle className="text-lg">{patient.name}</CardTitle>
                              <CardDescription>
                                {patient.age} yaş • {patient.gender} • Oda {patient.room}
                              </CardDescription>
                            </div>
                            <div className="text-right">
                              <div className="text-sm text-gray-500">ID: {patient.id}</div>
                            </div>
                          </div>
                        </CardHeader>
                        <CardContent className="space-y-4">
                          <div className="space-y-2">
                            <div className="text-base font-bold text-gray-800">Hastalık</div>
                            <div className="text-sm">{patient.disease}</div>
                          </div>

                          <div className="space-y-2">
                            <div className="text-base font-bold text-gray-800">Sorumlu Doktor</div>
                            <div className="text-sm">{patient.assigneddoctor}</div>
                          </div>

                          <div className="grid grid-cols-2 gap-4">
                            <div className="flex items-center gap-2 p-3 bg-red-50 rounded-lg">
                              <Heart className="h-4 w-4 text-red-500" />
                              <div>
                                <div className="text-xs text-gray-500">Kalp Atışı</div>
                                <div className="font-semibold">{patient.heartRate} BPM</div>
                              </div>
                            </div>

                            <div className="flex items-center gap-2 p-3 bg-amber-50 rounded-lg">
                              <Thermometer className="h-4 w-4 text-amber-500" />
                              <div>
                                <div className="text-xs text-gray-500">Vücut Sıcaklığı</div>
                                <div className="font-semibold">{patient.bodyTemp}°C</div>
                              </div>
                            </div>
                          </div>

                          <div className="pt-2">
                            <Link href={`/patient/${patient.id}`}>
                              <Button className="w-full" variant="outline">
                                Detayları Görüntüle
                              </Button>
                            </Link>
                          </div>
                        </CardContent>
                      </Card>
                    ))}
                  </div>
                )}
              </CardContent>
            </Card>
          </TabsContent>

          {/* Cihazlar Sekmesi */}
          <TabsContent value="devices" className="mt-4">
            <Card>
              <CardHeader>
                <div className="flex flex-col md:flex-row justify-between items-start md:items-center gap-4">
                  <div>
                    <CardTitle>Cihaz Listesi</CardTitle>
                    <CardDescription>Sistemdeki tüm cihazlar ve durumları</CardDescription>
                  </div>
                  <Dialog open={isAddDeviceOpen} onOpenChange={setIsAddDeviceOpen}>
                    <DialogTrigger asChild>
                      <Button size="sm">
                        <Plus className="h-4 w-4 mr-2" />
                        Yeni Cihaz Ekle
                      </Button>
                    </DialogTrigger>
                    <DialogContent className="sm:max-w-[425px]">
                      <DialogHeader>
                        <DialogTitle>Yeni Cihaz Ekle</DialogTitle>
                        <DialogDescription>Yeni cihaz bilgilerini girin.</DialogDescription>
                      </DialogHeader>
                      <form onSubmit={handleAddDevice}>
                        <div className="grid gap-4 py-4">
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="mac" className="text-right">
                              MAC Adresi
                            </Label>
                            <Input
                              id="mac"
                              placeholder="AA:BB:CC:DD:EE:FF"
                              value={newDevice.mac}
                              onChange={(e) => setNewDevice({ ...newDevice, mac: e.target.value })}
                              className="col-span-3"
                              required
                            />
                          </div>
                          <div className="grid grid-cols-4 items-center gap-4">
                            <Label htmlFor="ip" className="text-right">
                              IP Adresi
                            </Label>
                            <Input
                              id="ip"
                              placeholder="192.168.1.100"
                              value={newDevice.ip}
                              onChange={(e) => setNewDevice({ ...newDevice, ip: e.target.value })}
                              className="col-span-3"
                              required
                            />
                          </div>
                        </div>
                        <DialogFooter>
                          <Button type="button" variant="outline" onClick={() => setIsAddDeviceOpen(false)}>
                            İptal
                          </Button>
                          <Button type="submit">Cihaz Ekle</Button>
                        </DialogFooter>
                      </form>
                    </DialogContent>
                  </Dialog>
                </div>

                {/* Device Search and Filters */}
                <div className="flex flex-col md:flex-row gap-4 mt-4">
                  <div className="relative flex-1">
                    <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-4 w-4 text-gray-400" />
                    <Input
                      placeholder="MAC adresi veya hasta adına göre ara..."
                      value={deviceSearchTerm}
                      onChange={(e) => setDeviceSearchTerm(e.target.value)}
                      className="pl-10"
                    />
                  </div>
                  <div className="flex gap-2">
                    <Select value={deviceStatusFilter} onValueChange={setDeviceStatusFilter}>
                      <SelectTrigger className="w-[140px]">
                        <SelectValue placeholder="Durum" />
                      </SelectTrigger>
                      <SelectContent>
                        <SelectItem value="all">Tüm Durumlar</SelectItem>
                        {uniqueDeviceStatuses.map((status) => (
                          <SelectItem key={status} value={status}>
                            {status}
                          </SelectItem>
                        ))}
                      </SelectContent>
                    </Select>
                  </div>
                </div>
              </CardHeader>
              <CardContent>
                <div className="overflow-x-auto">
                  <table className="w-full">
                    <thead>
                      <tr className="border-b text-sm">
                        <th className="text-left font-medium p-4">MAC Adresi</th>
                        <th className="text-left font-medium p-4">IP Adresi</th>
                        <th className="text-left font-medium p-4">Atanan Hasta</th>
                        <th className="text-left font-medium p-4">Durum</th>
                        <th className="text-left font-medium p-4">Son Görülme</th>
                        <th className="text-left font-medium p-4">Pil Seviyesi</th>
                        <th className="text-left font-medium p-4">Firmware</th>
                        <th className="text-left font-medium p-4">İşlemler</th>
                      </tr>
                    </thead>
                    <tbody>
                      {filteredDevices.map((device) => (
                        <tr key={device.mac} className="border-b">
                          <td className="p-4 font-mono text-sm">{device.mac}</td>
                          <td className="p-4">{device.ip}</td>
                          <td className="p-4">{device.assignedPatient || "-"}</td>
                          <td className="p-4">
                            <span
                              className={`inline-flex items-center rounded-full px-2.5 py-0.5 text-xs font-medium ${
                                device.status === "Çevrimiçi"
                                  ? "bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200"
                                  : device.status === "Çevrimdışı"
                                    ? "bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200"
                                    : "bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200"
                              }`}
                            >
                              {device.status}
                            </span>
                          </td>
                          <td className="p-4">{device.lastSeen}</td>
                          <td className="p-4">
                            <div className="flex items-center gap-2">
                              <div className="w-16 bg-gray-200 rounded-full h-2">
                                <div
                                  className={`h-2 rounded-full ${
                                    device.batteryLevel > 50
                                      ? "bg-green-500"
                                      : device.batteryLevel > 20
                                        ? "bg-yellow-500"
                                        : "bg-red-500"
                                  }`}
                                  style={{ width: `${device.batteryLevel}%` }}
                                ></div>
                              </div>
                              <span className="text-sm">{device.batteryLevel}%</span>
                            </div>
                          </td>
                          <td className="p-4 text-sm">{device.firmwareVersion}</td>
                          <td className="p-4">
                            <Button variant="outline" size="sm">
                              Detay
                            </Button>
                          </td>
                        </tr>
                      ))}
                    </tbody>
                  </table>

                  {filteredDevices.length === 0 && (
                    <div className="text-center py-8 text-gray-500">Arama kriterlerinize uygun cihaz bulunamadı.</div>
                  )}
                </div>
              </CardContent>
            </Card>
          </TabsContent>

          {/* Uyarılar Sekmesi */}
          <TabsContent value="notifications" className="mt-4">
            <Card>
              <CardHeader>
                <div className="flex flex-col md:flex-row justify-between items-start md:items-center gap-4">
                  <div>
                    <CardTitle>Sistem Uyarıları</CardTitle>
                    <CardDescription>En son uyarılar ve bildirimler</CardDescription>
                  </div>
                  <Button size="sm" variant="outline">
                    Tümünü Okundu İşaretle
                  </Button>
                </div>
              </CardHeader>
              <CardContent>
                <div className="space-y-4">
                  {mockNotifications.map((notification) => (
                    <div key={notification.id} className="rounded-lg border p-4">
                      <div className="flex items-start justify-between">
                        <div className="flex items-start gap-3">
                          <div
                            className={`rounded-full p-2 ${
                              notification.severity === "Kritik"
                                ? "bg-red-100"
                                : notification.severity === "Uyarı"
                                  ? "bg-yellow-100"
                                  : "bg-blue-100"
                            }`}
                          >
                            {notification.severity === "Kritik" ? (
                              <Bell className="h-4 w-4 text-red-600" />
                            ) : notification.severity === "Uyarı" ? (
                              <Bell className="h-4 w-4 text-yellow-600" />
                            ) : (
                              <Bell className="h-4 w-4 text-blue-600" />
                            )}
                          </div>
                          <div className="flex-1">
                            <div className="flex items-center gap-2 mb-1">
                              <span className="font-medium">{notification.patientName}</span>
                              <span className="text-sm text-gray-500">• Oda {notification.room}</span>
                              <span
                                className={`inline-flex items-center rounded-full px-2 py-0.5 text-xs font-medium ${
                                  notification.severity === "Kritik"
                                    ? "bg-red-100 text-red-800"
                                    : notification.severity === "Uyarı"
                                      ? "bg-yellow-100 text-yellow-800"
                                      : "bg-blue-100 text-blue-800"
                                }`}
                              >
                                {notification.severity}
                              </span>
                            </div>
                            <p className="text-sm text-gray-700 mb-2">{notification.alert}</p>
                            <div className="flex items-center gap-4 text-xs text-gray-500">
                              <span>Cihaz: {notification.assigneddevicemac}</span>
                              <span>Zaman: {notification.timestamp}</span>
                            </div>
                          </div>
                        </div>
                        <div className="flex gap-2">
                          <Button variant="outline" size="sm">
                            Detay
                          </Button>
                          <Button variant="ghost" size="sm">
                            Kapat
                          </Button>
                        </div>
                      </div>
                    </div>
                  ))}

                  {mockNotifications.length === 0 && (
                    <div className="text-center py-8 text-gray-500">Henüz uyarı bulunmuyor.</div>
                  )}
                </div>
              </CardContent>
            </Card>
          </TabsContent>
        </Tabs>
      </main>
    </div>
  )
}
