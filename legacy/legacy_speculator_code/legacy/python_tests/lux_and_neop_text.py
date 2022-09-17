import time
import board
import busio
import adafruit_tca9548a
import adafruit_veml7700
import board
import neopixel
from time import sleep

# neopixels
neop = neopixel.NeoPixel(board.D21, 12)
pixel_num = 12
neop.brightness = 0.2
neop.auto_write = True
bright_f = 0.2
bright_r = 0.2

# multiplexer
i2c = busio.I2C(board.SCL, board.SDA)
tca = adafruit_tca9548a.TCA9548A(i2c)

# lux sensors
veml1 = adafruit_veml7700.VEML7700(tca[0])
veml2 = adafruit_veml7700.VEML7700(tca[1])
lux_min = 20
lux_max = 6000

def constrainLux(lux):
    return min(max(lux_min, lux), lux_max)

def mapLux(lux):
    lux_range = lux_max - lux_min
    return (lux - lux_min) / lux_range

def constrainAndMapLux(lux):
    return mapLux(constrainLux(lux))

while True:
    print("lux (front/back)     : ", veml1.lux, " | ", veml2.lux)
    bright_f = constrainAndMapLux(veml1.lux)
    bright_r = constrainAndMapLux(veml2.lux)
    print("lux mapped to bright : ", bright_f, " | ", bright_r)
    print("|-"*20)
    neop.fill((255,0,0));
    neop.brightness = bright_f
    time.sleep(2.5)
