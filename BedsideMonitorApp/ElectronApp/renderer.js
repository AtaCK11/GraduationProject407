const { contextBridge, ipcRenderer } = require('electron');
// This file is loaded by the renderer process
console.log("Renderer process started")

contextBridge.exposeInMainWorld('electronAPI', {
  onSerialData: (callback) => ipcRenderer.on('serial-data', (e, d) => callback(d)),
  onAvailablePorts: (callback) => ipcRenderer.on('available-ports', (e, d) => callback(d)),
  connectToPort: (portPath) => ipcRenderer.send('connect-port', portPath),
});

// You can add more renderer-specific code here
