# ESP32 T-Display Wireless Audit

Defensive wireless inventory and diagnostics toolkit for **authorized networks only**, built for the original LILYGO TTGO T-Display ESP32 (ST7789 240x135).

## MVP features
- Passive Wi-Fi network discovery: SSID, BSSID, RSSI, channel, advertised security mode.
- Passive BLE advertisement discovery: device name/address/RSSI.
- On-device TFT dashboard with two-button navigation.
- Local responsive Web UI served from ESP32 SoftAP `ESP32-AUDIT`.
- Web-triggered authorized scan and inventory tables.
- PlatformIO build configuration.

## Safety scope
This project intentionally excludes credential capture, deauthentication/disassociation, Evil Twin automation, brute-force/password attacks, packet injection attacks, or bypass tooling. Use only on networks/devices you own or have explicit permission to assess.

## Hardware
- LILYGO TTGO T-Display ESP32 (classic ESP32 version)
- ST7789 240x135 TFT
- Built-in buttons GPIO35 and GPIO0

## Build
1. Install VS Code + PlatformIO.
2. Open the `esp32-wireless-audit` directory.
3. Connect the T-Display via USB.
4. Run `PlatformIO: Build`, then `PlatformIO: Upload`.
5. Open serial monitor at 115200 if needed.

## Usage
- Device creates Wi-Fi AP: `ESP32-AUDIT`.
- Connect to it and browse to `192.168.4.1`.
- Use the on-device buttons to switch views and launch passive scans.

## Planned safe modules
- Channel utilization visualization and congestion scoring.
- Security posture flags for open/legacy configurations.
- Authorized SSID allowlist and audit profiles.
- JSON/CSV export.
- LittleFS scan history.
- BLE manufacturer/service UUID inspection.
- Optional GPS tagging when external GPS hardware is attached.
- Improved animated UX/UI and settings screens.
