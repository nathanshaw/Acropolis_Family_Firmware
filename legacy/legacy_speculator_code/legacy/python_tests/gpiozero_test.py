from gpiozero import LED
from gpiozero import Button
from time import sleep

led = LED(17)
v_pin = Button(5)

print("starting the program")
while True:
    if v_pin.is_pressed:
        led.on()
        print("LED ON")
    else:
        led.off()
