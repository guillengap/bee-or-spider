import sensor, image, time, os, tf, pyb
from machine import Pin

pin0 = Pin("PG1", Pin.OUT_PP, Pin.PULL_UP)
pin1 = Pin("PG12", Pin.OUT_PP, Pin.PULL_UP)

redLED = pyb.LED(1) # built-in red LED
greenLED = pyb.LED(2) # built-in green LED

sensor.reset()                         # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565)    # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)      # Set frame size to QVGA (320x240)
sensor.set_vflip(True)
sensor.set_hmirror(True)
sensor.set_windowing((240, 240))       # Set 240x240 window.
sensor.skip_frames(time=2000)          # Let the camera adjust.

labels, net = tf.load_builtin_model('bee_or_spider_v2')
found = False

def flashLED1(led): # Indicate with LED when target is detected
    found = True
    led.on()
    pin0.on()
    img.draw_string(5, 12, label)
    pyb.delay(1500)
    led.off()
    pin0.off()
    found = False


def flashLED2(led): # Indicate with LED when target is detected
    found = True
    led.on()
    pin1.on()
    img.draw_string(5, 12, label)
    pyb.delay(1500)
    led.off()
    pin1.off()
    found = False

clock = time.clock()

while not found:
    clock.tick()
    img = sensor.snapshot()
    for obj in tf.classify(net, img, min_scale=1.0, scale_mul=0.8, x_overlap=0.5, y_overlap=0.5):
        print("**********nPredictions at [x=%d,y=%d,w=%d,h=%d]" % obj.rect())
        img.draw_rectangle(obj.rect())
        predictions_list = list(zip(labels, obj.output()))
        for i in range(len(predictions_list)):
            confidence = predictions_list[i][1]
            label = predictions_list[i][0]
            print("%s = %f" % (label, confidence))
            if confidence > 0.8:
                if label == "bee":
                    print("It's a BEE")
                    #img.draw_string(5, 12, label)
                    flashLED1(greenLED)
                    #pin0.on()
                if label == "spider":
                    print("It's a SPIDER")
                    #img.draw_string(5, 12, label)
                    flashLED2(redLED)
                    #pin1.on()

    print(clock.fps(), "fps")
