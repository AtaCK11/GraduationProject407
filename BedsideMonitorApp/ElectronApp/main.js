const path = require("path");
process.chdir(__dirname);

const { app, BrowserWindow, ipcMain } = require("electron")
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const { list } = require('@serialport/list');

// Handle creating/removing shortcuts on Windows when installing/uninstalling
if (require("electron-squirrel-startup")) {
  app.quit()
}

// Keep a global reference of the window object to prevent garbage collection
let mainWindow
let port;
let parser;


async function createWindow() {
  // Create the browser window
  mainWindow = new BrowserWindow({
    width: 1920,
    height: 1080,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, "preload.js"),
    },
    backgroundColor: "#1a1a1a",
  })

  // Load the index.html file
  mainWindow.loadFile("index.html")
  

  mainWindow.webContents.once('did-finish-load', async () => {
    try {
      const ports = await list();
      console.log('Available ports:', ports);
      mainWindow.webContents.send('available-ports', ports);
    } catch (err) {
      console.error('Error listing ports:', err);
    }
  });



  // Open DevTools for debugging (comment out for production)
  mainWindow.webContents.openDevTools()
  
  // Log when window is ready
  mainWindow.webContents.on("did-finish-load", () => {
    console.log("Window loaded successfully")
  })

  // Handle window close
  mainWindow.on("closed", () => {
    mainWindow = null
  })
}

// Function to connect and parse data
function connectToPort(patha) {
    port = new SerialPort({
    path: 'COM'+patha,
    baudRate: 115200, // Adjust this to match your device
  });
  
  console.log("Trying to connect to " + "COM" + patha);

  parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

  parser.on('data', (line) => {
    const match = line.match(/\[SENSOR_DATA\] -> (\d+) \| ([\d.]+)/);
    if (match) {
      const sensorTag = parseInt(match[1], 10);
      const sensorValue = parseFloat(match[2]);
      mainWindow.webContents.send('serial-data', { sensorTag, sensorValue });
    }
  });


  port.on('error', (err) => {
    console.error('Serial port error:', err.message);
  });

  port.on('close', () => {
    console.log('Serial port closed');
  });
  
  console.log("Connected to " + "COM" + patha);
}

// Allow renderer to manually request a port connection
ipcMain.on('connect-port', (event, portPath) => {
  connectToPort(portPath);
});

// Create window when Electron is ready
app.whenReady().then(() => {
  console.log("Electron app is ready")
  createWindow()

  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow()
    }
  })
})

// Quit when all windows are closed (except on macOS)
app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit()
  }
})

// Log any uncaught exceptions
process.on("uncaughtException", (error) => {
  console.error("Uncaught Exception:", error)
})
