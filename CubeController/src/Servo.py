from machine import Pin, PWM

class Servo:
    def __init__(self, pin_number: int, freq: int = 50):
        pin = Pin(pin_number)
        self._pwm = PWM(pin, freq=freq)
        self._min_us = 500
        self._max_us = 2550
    
    def write_us(self, us: int):
        us = max(self._min_us, min(self._max_us, us))
        ns = int(us * 1000)
        self._pwm.duty_ns(ns)