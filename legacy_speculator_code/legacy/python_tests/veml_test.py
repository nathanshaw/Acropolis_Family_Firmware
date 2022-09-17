import time
import board
import busio
import adafruit_veml7700

i2c = busio.I2C(board.SCL, board.SDA)
veml = adafruit_veml7700.VEML7700(i2c)

while(True):
    print("ambiant light: ", veml.light)
    print("ux:", veml.lux)
    time.sleep(0.5)
