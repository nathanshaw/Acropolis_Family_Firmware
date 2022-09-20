import time
import board
import busio
import adafruit_tca9548a
import adafruit_veml7700

i2c = busio.I2C(board.SCL, board.SDA)

tca = adafruit_tca9548a.TCA9548A(i2c)

veml1 = adafruit_veml7700.VEML7700(tca[0])
veml2 = adafruit_veml7700.VEML7700(tca[1])

while True:
    print("lux (front/back): ", veml1.lux, " | ", veml2.lux)
    time.sleep(0.5)
