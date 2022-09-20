import board
import busio
import adafruit_tca9548a
import adafruit_veml7700
import neopixel
from time import sleep
from random import randint
from oscpy.server import OSCThreadServer

# lux sensors - veml7700
i2c = busio.I2C(board.SCL, board.SDA)
tca = adafruit_tca9548a.TCA9548A(i2c)
veml1 = adafruit_veml7700.VEML7700(tca[0])
veml2 = adafruit_veml7700.VEML7700(tca[1])
lux_min = 20
lux_max = 1000

# neopixels
neop = neopixel.NeoPixel(board.D21, 12)
pixel_num = 12
neop.brightness = 0.6
# neop.auto_write = True
bright = 0.2 # the brightness of the entire strip
bright_f = 0.2
bright_r = 0.2
bright_min = 0.02
bright_max = 0.8

def constrainLux(lux, lmin=lux_min, lmax=lux_max):
    """
    This function constrains the lux value according to
    lmin and lmax.If lux is larger than lmax then lmax will be
    the output. If lux is less than lmin then lmin will be the
    output

    INPUTS
    -------------------
    lux - the lux value as captured from the veml7700 sensor
    lmin - the lowest lux value which will be allowed
    lmax - the maximum lux value which will be allowed
    """
    return min(max(lmin, lux), lmax)

def mapLux(lux, lmin=lux_min, lmax=lux_max):
    """
    This function returns a value between 0 and 1, where 0 equals lmin
    and 1 equals lmax.

    INPUTS
    -------------------
    lux - the lux value as captured from the veml7700 sensor
    lmin - the lowest lux value which is allowed
    lmax - the maximum lux value which is allowed
    """
    lux_range = lux_max - lux_min
    return (lux - lux_min) / lux_range

def constrainBright(bright, bmin=bright_min, bmax=bright_max):
    """
    This function takes in a bright value (0.0-1.0) and returns
    a constrained brightness value between bmin and bmax

    INPUTS
    -------------------
    bright - a brightness value between 0 and 1.0
    bmin - the lowest brightness output value which is allowed
    bmax - the maximum brightness output value which is allowed
    """
    return min(max(bright, bmin), bmax)

def constrainAndMapLux(lux, lmin=lux_min, lmax=lux_max):
    """
    This function first constrains the lux value according to
    lmin and lmax before passing the constrained value into the
    map function which returns a value between 0 and 1, where 0 equals lmin
    and 1 equals lmax

    INPUTS
    -------------------
    lux - the lux value as captured from the veml7700 sensor
    lmin - the lowest lux value which will be allowed
    lmax - the maximum lux value which will be allowed
    """
    return mapLux(constrainLux(lux, lmin, lmax))

def click(intensity=1.0, side="both"):
    """
    This function triggers a "click" visualisation for the neopixels
    Select the side of the click, default is "both", but "front" and
    "rear" are also options

    INPUTS
    -------------------
    side - which side the click will appear on, options include the "front"
    "rear" and "both"
    """
    if side is "both":
        value = int(255*intensity)
        neop.fill((value, value, value))
        neop.show()
    elif side is "front":
        value = int(255*intensity)
        for i in range(0,6):
            neop[i] = (value, value, value)
        neop.show()
    elif side is "rear":
        value = int(255*intensity)
        for i in range(6,12):
            neop[i] = (value, value, value)
        neop.show()
    else:
        print("ERROR, click needs to be called with 'both', 'front', or 'rear' as an argument")

def neopFill(frontb, rearb, side="all"):
    """
    TODO

    INPUTS
    -------------------
    side which neopixels to fill
    "all"   - fill the front and back using bright_f and bright_r as a guide
    "both"  - fill the front and back using an average of bright_f and bright_f as a guide
    "front" - fill only the front pixels according to bright_f
    "rear"  - fill only the rear pixels according to bright_r
    """
    bf = (int(255*frontb))
    br = (int(255*rearb))
    # print("bf : ", bf, " br : ", br)
    if side is "all":
        for i in range(0,6):
            neop[i] = (bf, 0, 0)
        for i in range(6,12):
            neop[i] = (br, 0, 0)
        neop.show()
    elif side is "both":
        b = (bf + br)*0.5
        neop.fill((b, 0, 0))
        neop.show()
    elif side is "front":
        for i in range(0,6):
            neop[i] = (bf, 0, 0)
        neop.show()
    elif side is "rear":
        for i in range(6,12):
            neop[i] = (br, 0, 0)
        neop.show()
    else:
        print("ERROR, neopFill needs to be called with 'both', 'front', or 'rear' as an argument")

def recvRMS(rms):
    """
    Receive rms data from chuck and use it to adjust neopixel brightness

    RMS should be scaled between 0.0 and 1.0 when received
    """
    # TODO not sure if I need to be reading the lux sensors each time msg is received
    rms = min(rms, 1.0)
    # print("RMS message received: ", rms)
    bright_f = constrainBright(constrainAndMapLux(veml1.lux))
    bright_r = constrainBright(constrainAndMapLux(veml2.lux))
    neopFill(bright_f, bright_r)


def recvClick(amp):
    """
    Receive click event data from ChucK and use it to trigger a "click" event
    """
    print("Click message received")
    click(amp)

def testLuxToBrightness(verbose=0):
    bright_f = constrainBright(constrainAndMapLux(veml1.lux))
    bright_r = constrainBright(constrainAndMapLux(veml2.lux))
    neopFill(bright_f, bright_r)
    if verbose > 0:
        print("lux (front/back)     : ", round(veml1.lux, 3), " | ", round(veml2.lux, 3))
        print("lux mapped to bright : ", round(bright_f, 3),  " | ", round(bright_r, 3))

while __name__ == "__main__":
    osc = OSCThreadServer()
    oscReceiver = osc.listen(address='127.0.0.1', port=6450, default=True)
    osc.bind(b'/rms', recvRMS)
    osc.bind(b'/click', recvClick)
    while True:
        # just keep the program running
        sleep(100)
    """
    while True:
        testLuxToBrightness()
        if randint(0,40) < 1:
            print("CLICK MESSAGE")
            click()
        sleep(0.1)
    """

