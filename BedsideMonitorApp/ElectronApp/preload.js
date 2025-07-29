const { contextBridge, ipcRenderer } = require('electron');

// This runs before the renderer loads
console.log("Preload script loaded");

// Expose your custom API safely
contextBridge.exposeInMainWorld('electronAPI', {
  getAppVersion: () => require("electron").app.getVersion(), // careful: this may not work as-is in preload
  sendVitalSigns: (data) => {
    console.log("Vital signs data:", data);
  },
  onSerialData: (callback) => ipcRenderer.on('serial-data', (e, d) => callback(d)),
  onAvailablePorts: (callback) => ipcRenderer.on('available-ports', (e, d) => callback(d)),
  connectToPort: (portPath) => ipcRenderer.send('connect-port', portPath),
});
