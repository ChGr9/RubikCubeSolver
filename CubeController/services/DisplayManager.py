from machine import Pin, I2C
from sh1106 import SH1106_I2C
from writer import Writer
import font10 as font
from math import pi, sin, cos

class DisplayManager:
    def __init__(self, scl: int, sda: int):
        i2c = I2C(0, scl=Pin(scl), sda=Pin(sda))
        self.display = SH1106_I2C(128, 64, i2c)
        self.display.rotate(True)
        self.writer = Writer(self.display, font)
        self.clear()

    def clear(self):
        self.display.fill(0)

    def show(self):
        self.display.show()

    def write(self, text: str, y: int = 0):
        Writer.set_textpos(self.display, y, 0)
        self.writer.printstring(text)

    def write_centered(self, text: str, y: int = None):
        text_width = self.writer.stringlen(text)
        x = (self.display.width - text_width) // 2
        x = max(0, x)
        if y is None:
            char_height = self.writer.font.height()
            y = (self.display.height - char_height) // 2
        Writer.set_textpos(self.display, y, x)
        self.writer.printstring(text)
    
    def draw_loading(self, progress: int):
        x_center = self.display.width // 2
        y_center = self.display.height // 2 + 10
        radius = 15
        num_dots = 4
        arc_length = 180

        for i in range(num_dots):
            angle = (progress + i * (arc_length / num_dots)) * pi  / 180
            x = int(x_center + radius * cos(angle))
            y = int(y_center + radius * sin(angle))

            self.draw_filled_circle(x, y, i + 1)
        
        progress += 1
    
    def draw_filled_circle(self, cx: int, cy: int, radius: int):
        for x in range(-radius, radius + 1):
            for y in range(-radius, radius + 1):
                if x ** 2 + y ** 2 <= radius ** 2:
                    self.display.pixel(cx + x, cy + y, 1)
