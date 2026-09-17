import express from 'express';
import os from 'node:os';
import path from 'node:path';

const PORT = 3000;
const HOST = '0.0.0.0';
const CLIENT_DIR = path.join(import.meta.dirname, '..', 'client');
const OFFLINE_AFTER_MS = 3000;

const device = {
  lastSeenAt: 0,
  telemetry: null,
  ledWanted: false,
};

const app = express();

app.use(express.json());
app.use(express.static(CLIENT_DIR));

function isOnline() {
  return Date.now() - device.lastSeenAt < OFFLINE_AFTER_MS;
}

app.get('/api/ping', (req, res) => {
  res.json({ message: 'pong', time: new Date().toISOString() });
});

app.post('/api/telemetry', (req, res) => {
  const { uptimeMs, rssi, led } = req.body ?? {};

  if (typeof uptimeMs !== 'number' || typeof rssi !== 'number' || typeof led !== 'boolean') {
    return res.status(400).json({ error: 'Expected { uptimeMs: number, rssi: number, led: boolean }' });
  }

  if (!isOnline()) {
    console.log(`ESP32 online (${req.ip})`);
  }

  device.lastSeenAt = Date.now();
  device.telemetry = { uptimeMs, rssi, led };

  res.json({ led: device.ledWanted });
});

app.get('/api/device', (req, res) => {
  res.json({
    online: isOnline(),
    lastSeenAt: device.lastSeenAt ? new Date(device.lastSeenAt).toISOString() : null,
    telemetry: device.telemetry,
    ledWanted: device.ledWanted,
  });
});

app.post('/api/led', (req, res) => {
  const { on } = req.body ?? {};

  if (typeof on !== 'boolean') {
    return res.status(400).json({ error: 'Expected { on: boolean }' });
  }

  device.ledWanted = on;
  console.log(`LED -> ${on ? 'on' : 'off'} (from page)`);

  res.json({ ledWanted: device.ledWanted });
});

function lanAddresses() {
  return Object.values(os.networkInterfaces())
    .flat()
    .filter((iface) => iface.family === 'IPv4' && !iface.internal)
    .map((iface) => iface.address);
}

app.listen(PORT, HOST, (error) => {
  if (error) throw error;

  console.log(`Server is running on http://localhost:${PORT}`);
  for (const address of lanAddresses()) {
    console.log(`  for ESP32: http://${address}:${PORT}`);
  }
});
