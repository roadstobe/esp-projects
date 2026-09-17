const POLL_INTERVAL_MS = 1000;

const els = {
  badge: document.querySelector('#badge'),
  serverStatus: document.querySelector('#server-status'),
  boardUptime: document.querySelector('#board-uptime'),
  boardDetails: document.querySelector('#board-details'),
  ledState: document.querySelector('#led-state'),
  ledButton: document.querySelector('#led-button'),
  ledHint: document.querySelector('#led-hint'),
};

// Last response from GET /api/device, or null while the server is unreachable
let device = null;
let sendingCommand = false;

function formatUptime(ms) {
  const totalSeconds = Math.floor(ms / 1000);
  const hours = Math.floor(totalSeconds / 3600);
  const minutes = Math.floor((totalSeconds % 3600) / 60);
  const seconds = totalSeconds % 60;

  if (hours > 0) return `${hours} h ${minutes} min`;
  if (minutes > 0) return `${minutes} min ${seconds} s`;
  return `${seconds} s`;
}

// The server sends UTC ("...Z"); toLocaleTimeString converts it to the browser's time zone
function formatTime(isoString) {
  return new Date(isoString).toLocaleTimeString('en-GB');
}

function render() {
  const online = device?.online ?? false;
  const telemetry = device?.telemetry;

  els.badge.textContent = online ? 'online' : 'offline';
  els.badge.classList.toggle('badge--online', online);

  els.serverStatus.textContent = device ? 'Running' : 'Unreachable';

  if (online) {
    els.boardUptime.textContent = formatUptime(telemetry.uptimeMs);
    els.boardDetails.textContent = `Wi-Fi ${telemetry.rssi} dBm · ${formatTime(device.lastSeenAt)}`;
  } else {
    els.boardUptime.textContent = 'offline';
    els.boardDetails.textContent = device?.lastSeenAt
      ? `Last seen at ${formatTime(device.lastSeenAt)}`
      : "The board hasn't connected yet";
  }

  const ledWanted = device?.ledWanted ?? false;

  els.ledButton.textContent = ledWanted ? 'Turn off' : 'Turn on';
  els.ledButton.disabled = !device || sendingCommand;
  els.ledState.textContent = online ? (telemetry.led ? 'On' : 'Off') : '—';

  if (!device) {
    els.ledHint.textContent = 'Server unreachable';
  } else if (!online) {
    els.ledHint.textContent = 'The command will run when the board connects';
  } else if (telemetry.led !== ledWanted) {
    // The board picks up the command only with its next request, up to a second later
    els.ledHint.textContent = 'Waiting for the board to pick up the command…';
  } else {
    els.ledHint.textContent = 'LED state on the board';
  }
}

async function poll() {
  try {
    const res = await fetch('/api/device');
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    device = await res.json();
  } catch {
    device = null;
  }

  render();
  // setTimeout after the request instead of setInterval, so slow requests never pile up
  setTimeout(poll, POLL_INTERVAL_MS);
}

els.ledButton.addEventListener('click', async () => {
  sendingCommand = true;
  render();

  try {
    const res = await fetch('/api/led', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ on: !device.ledWanted }),
    });
    if (!res.ok) throw new Error(`HTTP ${res.status}`);

    const { ledWanted } = await res.json();
    if (device) device.ledWanted = ledWanted;
  } catch (error) {
    console.error('LED command failed:', error);
  } finally {
    sendingCommand = false;
    render();
  }
});

poll();
