import board
import neopixel
from time import sleep

neop = neopixel.NeoPixel(board.D21, 12)

pixel_num = 12
neop.brightness = 0.2

neop.auto_write = True

neop.fill((0, 255, 0))
