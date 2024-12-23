from services.DisplayManager import DisplayManager
import time
import network

def connect_wifi(ssid: str, password: str, display: DisplayManager):
    station = network.WLAN(network.STA_IF)
    station.active(True)
    
    if not station.isconnected():
        station.connect(ssid, password)

        progress = 0
        timeout = 30_000 # 30 seconds
        start_time = time.ticks_ms()
        while not station.isconnected() and time.ticks_diff(time.ticks_ms(), start_time) < timeout:
            display.clear()
            display.write_centered("Connecting", 0)
            display.draw_loading(progress)
            display.show()
            time.sleep_ms(50)
            progress += 10
    
    if station.isconnected():
        display.clear()
        display.write_centered("WiFi connected")
        display.show()
    else:
        display.clear()
        display.write("Failed to connect to WiFi")
        display.show()

def main():
    display = DisplayManager(33, 32)
    connect_wifi("SSID", "PASSWORD", display)


if __name__ == "__main__":
    main()