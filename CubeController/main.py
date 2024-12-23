from src import DisplayManager, ServoManager, PageManager
from pages import ListMenuPage
import time
import network

def connect_wifi(ssid: str, password: str, display_manager: DisplayManager) -> bool:
    station = network.WLAN(network.STA_IF)
    station.active(True)
    
    if not station.isconnected():
        station.connect(ssid, password)

        progress = 0
        timeout = 30_000 # 30 seconds
        start_time = time.ticks_ms()
        while not station.isconnected() and time.ticks_diff(time.ticks_ms(), start_time) < timeout:
            display_manager.clear()
            display_manager.write_centered("Connecting", 0)
            display_manager.draw_loading(progress)
            display_manager.show()
            time.sleep_ms(50)
            progress += 10
    
    if station.isconnected():
        display_manager.clear()
        display_manager.write_centered("WiFi connected")
        display_manager.show()
        return True
    else:
        display_manager.clear()
        display_manager.write("Failed to connect to WiFi")
        display_manager.show()
        return False

def main():
    display_manager = DisplayManager(33, 32)
    if not connect_wifi("SSID", "PASSWORD", display_manager):
         return
    servo_manager = ServoManager()
    page_manager = PageManager()

    page_manager.setPage(ListMenuPage(page_manager, display_manager))


if __name__ == "__main__":
    main()