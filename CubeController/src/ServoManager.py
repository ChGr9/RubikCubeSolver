from .Servo import Servo
import time

class ServoManager:
    def __init__(self):
        self._boot_servo = Servo(27)
        self._BOOT_REST_POS = 1166
        self._BOOT_MID_POS = 1050
        self._BOOT_KICK_POS = 940
        self._is_boot_up = False

        self._holder_servo = Servo(25)
        self._HOLDER_POS = [500, 1200, 1930, 2540]
        self._holder_pos_index = 0

        self._handle_servo = Servo(12)
        self._HANDLE_OPEN_POS = 1166
        self._HANDLE_CLOSE_POS = 1920
        self.resetServos()

    def lowerBoot(self):
        self._is_boot_up = False
        self._boot_servo.write_us(self._BOOT_REST_POS)
        time.sleep(1)
    
    def bootKick(self):
        if self._is_boot_up:
            self.lowerBoot()
        self._boot_servo.write_us(self._BOOT_KICK_POS)
        time.sleep_ms(400)
        self._boot_servo.write_us(self._BOOT_MID_POS)
        time.sleep_ms(600)
        self._boot_servo.write_us(self._BOOT_KICK_POS)
        time.sleep_ms(400)
        self._boot_servo.write_us(self._BOOT_REST_POS)
        time.sleep(1)
    
    def bootHalfKick(self):
        if self._is_boot_up:
            self.lowerBoot()
        self._boot_servo.write_us(self._BOOT_KICK_POS)
        time.sleep_ms(400)
        self._boot_servo.write_us(self._BOOT_MID_POS)
        time.sleep_ms(600)
        self._boot_servo.write_us(self._BOOT_KICK_POS)
        time.sleep(1)
        self._is_boot_up = True
    
    def handleOpen(self):
        if self._is_boot_up:
            self.lowerBoot()
        self._handle_servo.write_us(self._HANDLE_OPEN_POS)
        time.sleep(1)
    
    def handleClose(self):
        if self._is_boot_up:
            self.lowerBoot()
        self._handle_servo.write_us(self._HANDLE_CLOSE_POS)
        time.sleep(1)
    
    def holderTurn(self, times: int):
        if self._is_boot_up:
            self.lowerBoot()
        self._holder_pos_index = (self._holder_pos_index + times) % len(self._HOLDER_POS)
        self._holder_servo.write_us(self._HOLDER_POS[self._holder_pos_index])
        time.sleep_ms(500)
    
    def resetServos(self):
        self._boot_servo.write_us(self._BOOT_REST_POS)
        self._is_boot_up = False
        self._handle_servo.write_us(self._HANDLE_OPEN_POS)
        time.sleep(1)
        self._holder_pos_index = 0
        self._holder_servo.write_us(self._HOLDER_POS[0])
        time.sleep_ms(500)
