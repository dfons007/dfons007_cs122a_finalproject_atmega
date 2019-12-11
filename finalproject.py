from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont
from PIL import ImageFilter

from lib_tft24T import TFT24T
import RPi.GPIO as GPIO
import serial
import select
import v4l2capture
import spidev
from time import sleep

def capture(mode):
    video = v4l2capture.Video_device('/dev/video0')
    size_x, size_y = video.set_format(320,240)
    video.create_buffers(1)
    video.queue_all_buffers()
    video.start()
    select.select((video,),(),())
    image_data = video.read()
    video.close()
    image = Image.frombytes("RGB",(size_x,size_y), image_data)
    image.save("games.jpg")
    if(mode == "1"):
        image = Image.open("games.jpg")
        image = image.convert('1')
        image.save("games.jpg")
    elif (mode== "L"):
        image = Image.open("games.jpg")
        image = image.convert('L')
        image.save("games.jpg")

def createSPI(device):
    #Obj
    spi = spidev.SpiDev()
    #Open Spi
    spi.open(0,device)
    spi.max_speed_hz=1000000
    # set mode
    spi.mode=0
    return spi

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(4, GPIO.IN)
GPIO.setup(17, GPIO.OUT)

image = Image.open("games.jpg")
image = image.convert('L')
image.save("games.jpg")

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
GPIO.output(17, False)
spiatmega = createSPI(1)

while 1:
  state = spiatmega.readbytes(1)
  GPIO.output(17, False)
  print(state)
  if(state[0] == 10 or state[0]==11 or state[0]==12):
    #get imag
    if(state[0]==10):
        capture("RGB")
    elif state[0]==11:
        capture("L")
    elif state[0]==12:
        capture("1")
    # Do image processing
    print(spiatmega.readbytes(1))
    TFT.clear()
    print('Loading image...')
    image = Image.open('games.jpg')
    # Resize the image and rotate it so it's 240x320 pixels.
    image = image.rotate(90,0,1).resize((240, 320))
    # Draw the image on the display hardware.
    print('Drawing image')
    TFT.display(image)
    print("done")
    sleep(1)
    spiatmega.writebytes([0xFF])
  else:
    print('Signal is no longer high')
  sleep(1)
