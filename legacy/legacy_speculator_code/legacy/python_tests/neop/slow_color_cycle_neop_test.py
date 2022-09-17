import board
import neopixel
from time import sleep

neop = neopixel.NeoPixel(board.D21, 12)

pixel_num = 12
neop.brightness = 0.2

neop.auto_write = True


if __name__ == '__main__':
    while True:
        neop.fill((255, 0, 0))
        print("red - first value")
        sleep(30)
        neop.fill((0, 255, 0))
        print("green - second value")
        sleep(30)
        neop.fill((0, 0, 255))
        print("blue - third value")
        sleep(30)
