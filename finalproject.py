from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

from lib_tft24T import TFT24T
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(4, GPIO.IN)
GPIO.setup(17, GPIO.OUT)

import spidev
from time import sleep

   
# Raspberry Pi configuration.
#For LCD TFT SCREEN:
DC = 24
RST = 25
LED = 15


# Create TFT LCD/TOUCH object:
TFT = TFT24T(spidev.SpiDev(), GPIO, landscape=False)
# If landscape=False or omitted, display defaults to portrait mode
# This demo can work in landscape or portrait


# Initialize display.
TFT.initLCD(DC, RST, LED)
# If rst is omitted then tie rst pin to +3.3V
# If led is omitted then tie led pin to +3.3V

# Get the PIL Draw object to start drawing on the display buffer.  
draw = TFT.draw()

#PYTHON FSM
state = GPIO.input(4)
while 1:
  state = GPIO.input(4)
  if(state):
    GPIO.output(17, False)
    TFT.clear()
    print('Loading image...')
    image = Image.open('bitch.jpg')
    # Resize the image and rotate it so it's 240x320 pixels.
    image = image.rotate(90,0,1).resize((240, 320))
    # Draw the image on the display hardware.
    print('Drawing image')
    TFT.display(image)
  else:
    TFT.clear()
    GPIO.output(17,True)
    print('Signal is no longer high')
  sleep(1)
