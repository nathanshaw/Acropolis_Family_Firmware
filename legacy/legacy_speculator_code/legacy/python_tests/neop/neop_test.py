import board
import neopixel
from time import sleep

neop = neopixel.NeoPixel(board.D21, 12)

pixel_num = 12
neop.brightness = 0.2

neop.auto_write = True

neop.fill((0, 255, 0))

def cycleColors(t):
    print("red")
    for i in range (0,255):
        neop.fill((i,0,0))
        sleep(t)
    print("reds/greens")
    for i in range(0,255):
        neop.fill((255 - i, i,0))
        sleep(t)
    print("greens/blues")
    for i in range(0,255):
        neop.fill((0,255-i,i))
        sleep(t)
    print("blues/reds")
    for i in range(0,255):
        neop.fill((i,0,255-i))
        sleep(t)
    neop.fill((0,0,0))

def cycleOneByOne(t):
    print("cycle one by one (red): ", t)
    for i in range (0,255, 25):
        for p in range(0, pixel_num):
            for pp in range(0, pixel_num):
                if pp == p:
                    neop[pp] = (i,0,0)
                else:
                    neop[pp] = (0,0,0)
                sleep(t)
    print("cycle one by one (reds/greens): ", t)
    for i in range (0,255, 25):
        for p in range(0, pixel_num):
            for pp in range(0, pixel_num):
                if pp == p:
                    neop[pp] = (255-i,i,0)
                else:
                    neop[pp] = (0,0,0)
                sleep(t)
    print("cycle one by one (greens/blues): ", t)
    for i in range (0,255, 25):
        for p in range(0, pixel_num):
            for pp in range(0, pixel_num):
                if pp == p:
                    neop[pp] = (0,255-i,i)
                else:
                    neop[pp] = (0,0,0)
                sleep(t)
    print("cycle one by one (blues/reds): ", t)
    for i in range (0,255, 25):
        for p in range(0, pixel_num):
            for pp in range(0, pixel_num):
                if pp == p:
                    neop[pp] = (i,0,255-i)
                else:
                    neop[pp] = (0,0,0)
                sleep(t)
    neop.fill((0,0,0))

if __name__ == '__main__':
    cycleColors(0.01)
    cycleOneByOne(0.01)
