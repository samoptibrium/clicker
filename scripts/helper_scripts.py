import clickeroo # The embedded clickeroo bindings module
import pyautogui # pip install pyautogui
import pyttsx3 # pip install pyttsx3
import random
import time
import winsound

def processEvents():
    clickeroo.processEvents()

# Mouse tweening
def getTween():
    return pyautogui.easeInOutQuad

# Mouse motion duration
def getDuration(x,y):
    return 1

## TODO helpers to draw to the graphics scene e.g. click target position memory, rectangle target areas
## TODO helpers to draw status/task strings to the graphics scene e.g. what am i doing now/next

def inRect(px, py, x1, y1, x2, y2):
    return px >= x1 and px <= x2 and py >= y1 and py <= y2

def moveOutsideRect(x1, y1, x2, y2):
    r = random.randint(0,3)
    offset = random.randint(50,100)
    if r == 0:
        moveToPx(x1 - offset, random.randint(y1, y2))
    elif r == 1:
        moveToPx(x2 + offset, random.randint(y1, y2))
    elif r == 2:
        moveToPx(random.randint(x1, x2), y1 - offset)
    elif r == 3:
        moveToPx(random.randint(x1, x2), y2 + offset)

def moveOutsideArea():
    moveOutsideRect(clickeroo.areaX1(), clickeroo.areaY1(), clickeroo.areaX2(), clickeroo.areaY2())

def moveToPx(x, y):
    pyautogui.moveTo(x, y, duration=getDuration(x, y), tween=getTween())
    processEvents()

def clickPointPx(x, y):
    pyautogui.click(x, y, duration=getDuration(x, y), tween=getTween())
    processEvents()

def clickAreaPx(x1, y1, x2, y2):
    pyautogui.click(x1, y2, duration=getDuration(x1, y1), tween=getTween())
    processEvents()
    
def moveTo(x, y):
    moveToPx(clickeroo.percentToMouseX(x), clickeroo.percentToMouseY(y))

def clickPoint(x, y):
    clickPointPx(clickeroo.percentToMouseX(x), clickeroo.percentToMouseY(y))

def clickArea(x1, y1, x2, y2):
    clickAreaPx(clickeroo.percentToMouseX(x1), clickeroo.percentToMouseY(y1), clickeroo.percentToMouseX(x2), clickeroo.percentToMouseY(y2))

def sleep(sec):
    total = 0
    part = sec / 10.0
    while total < sec:
        total += part
        time.sleep(part)
        processEvents()

def testArea(x1, y1, x2, y2, r, g, b, diff):
    return clickeroo.testArea(x1, y1, x2, y2, r, g, b, diff)

def errorBeep(frequency = 500, duration = 1000):
    winsound.Beep(frequency, duration)
    processEvents()

def speak(what):
    engine = pyttsx3.init()
    engine.say(what)
    engine.runAndWait()
    processEvents()

def log(what):
    clickeroo.log(what)
    processEvents()