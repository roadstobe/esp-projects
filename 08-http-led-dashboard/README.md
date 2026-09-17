# 08 · HTTP LED dashboard

The board talks to a Node.js (Express) server over plain HTTP. Every second it sends its state
(uptime, Wi-Fi signal, LED) and gets the LED command back in the response. A web page served
by the same server shows that data and has a button that switches the LED on the board.

This is the **HTTP polling** version: the board and the page keep asking the server, and the
server only answers. It is the simplest way to connect a board to a server, and the baseline
to compare other transports against (WebSocket, MQTT).

**Goal:** learn the basics of client-server communication: connecting to Wi-Fi, HTTP requests
from the ESP32 (`HTTPClient`), JSON on both sides (ArduinoJson, Express), and a web page that
polls the server.

**Board:** ESP32-S3-DevKitC-1 (N16R8)

## Parts

- LED
- 220 Ω resistor

## Wiring

| ESP32-S3 | Component |
|----------|-----------|
| GND | LED cathode (short leg) |
| GPIO5 | 220 Ω resistor → LED anode (long leg) |

## How it works

```
ESP32 ── POST /api/telemetry {uptimeMs, rssi, led} ──► server
      ◄──────────────────────────────── {led} ─────┘

page  ── GET /api/device (every second) ────────────► server
page  ── POST /api/led {on} ────────────────────────► server
```

1. Every second (`TELEMETRY_INTERVAL_MS`) the board sends `POST /api/telemetry` with its state.
2. The server saves it and answers with the LED state the page asked for.
3. If that differs from the real LED state, the board switches the LED. Its next report
   carries the new state, which is how the page knows the command was carried out.
4. The page polls `GET /api/device` every second. The board counts as online if it
   reported within the last 3 s.
5. The LED button sends `POST /api/led`. The server can't reach the board by itself, so the
   board picks the command up only with its next request, up to a second later.

## API

| Method | Path | Called by | Body → response |
|--------|------|-----------|-----------------|
| GET | `/api/ping` | curl, for a quick check | — → `{ message, time }` |
| POST | `/api/telemetry` | ESP32 | `{ uptimeMs, rssi, led }` → `{ led }` |
| GET | `/api/device` | page | — → `{ online, lastSeenAt, telemetry, ledWanted }` |
| POST | `/api/led` | page | `{ on }` → `{ ledWanted }` |

## Code structure

```
src/main.cpp  ── HTTP ──►  server/server.js  ◄── HTTP ──  client/ (browser)
```

| File | Role |
|------|------|
| `src/main.cpp` | Connects to Wi-Fi, sends the state every second, switches the LED by the response |
| `include/secrets-template.h` | Template for `include/secrets.h` with the Wi-Fi name and password |
| `server/server.js` | Express server: keeps the latest board state in memory, serves the API and the page |
| `client/index.html`, `client/style.css` | Dashboard layout |
| `client/app.js` | Polls the server every second, updates the cards, sends the LED command |

## HTTP polling: pros and cons

Pros:

- Easy to write and debug. Every exchange is one request and one response, and any of them
  can be repeated with `curl`.
- No connection stays open. If Wi-Fi or the server drops, one request fails and the next one
  works again, with no reconnect logic.

Cons:

- The server can't send anything by itself. A command waits for the board's next request,
  so the LED reacts up to `TELEMETRY_INTERVAL_MS` late.
- Requests go out every second even when nothing changes. A shorter interval gives faster
  reactions but more traffic and power use.
- The page polls too, so the data it shows can be up to about 2 s old.

## Notes

- **`localhost` doesn't work on the board.** For the ESP32 it means the board itself, so
  `SERVER_URL` needs the computer's LAN address, printed by the server as `for ESP32:`.
  The router may give the computer a new address later, so check that line if the board
  stops connecting.
- The ESP32 works only with **2.4 GHz** Wi-Fi. The board and the computer must be on the same
  network. **Dots in Serial that never stop** mean a wrong Wi-Fi name or password, or a 5 GHz network.
- **`connection refused` in Serial** means the board couldn't open a connection at all: the
  server isn't running, `SERVER_URL` has the wrong address, or the macOS firewall blocks
  incoming connections to Node (it asks about it the first time).
- **The server holds the wanted LED state; the board just follows it.** After a board restart
  the LED starts off and the first response turns it back on. The server keeps everything in
  memory, so after a server restart the command resets to off.
- The HTTP timeouts are 2 s. With the default 5 s, `loop()` would freeze that long on every
  request while the server is unreachable.
- The firmware sends `Content-Type: application/json`. Without it `express.json()` skips the
  body, and the server answers `400`.

## Try it

1. Copy `include/secrets-template.h` to `include/secrets.h` and fill in the Wi-Fi name and
   password. `secrets.h` is in `.gitignore`.
2. Start the server:
   ```sh
   cd server
   npm install
   npm run dev      # restarts on file changes
   ```
3. Put the `for ESP32:` address from the server output into `SERVER_URL` in `src/main.cpp`.
4. Flash the board and open the serial monitor:
   ```sh
   pio run -t upload -t monitor
   ```
5. Open http://localhost:3000 and press the LED button. A phone on the same Wi-Fi can open the
   `for ESP32:` address.

Serial output:

```
LED off
Connecting to MyWiFi.....
Connected, ESP32 IP address: 192.168.0.105
Server connected
LED on
```
