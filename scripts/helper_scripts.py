import clickeroo # The embedded clickeroo bindings module
import inspect
import pyautogui # pip install pyautogui
import pyttsx3 # pip install pyttsx3
import random
import time
import winsound

def dump_args(func):
    def wrapper(*args, **kwargs):
        func_args = inspect.signature(func).bind(*args, **kwargs).arguments
        func_args_str = ", ".join(map("{0[0]} = {0[1]!r}".format, func_args.items()))
        what = f"{func.__module__}.{func.__qualname__} ( {func_args_str} )"
        clickeroo.log(what)
        processEvents()
        return func(*args, **kwargs)
    return wrapper

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

@dump_args
def clickPointPx(x, y):
    pyautogui.click(x, y, duration=getDuration(x, y), tween=getTween())
    processEvents()

@dump_args
def clickAreaPx(x1, y1, x2, y2):
    pyautogui.click(x1, y2, duration=getDuration(x1, y1), tween=getTween())
    processEvents()
    
@dump_args
def moveTo(x, y):
    moveToPx(clickeroo.percentToMouseX(x), clickeroo.percentToMouseY(y))

@dump_args
def clickPoint(x, y):
    clickPointPx(clickeroo.percentToMouseX(x), clickeroo.percentToMouseY(y))

@dump_args
def clickArea(x1, y1, x2, y2):
    clickAreaPx(clickeroo.percentToMouseX(x1), clickeroo.percentToMouseY(y1), clickeroo.percentToMouseX(x2), clickeroo.percentToMouseY(y2))

@dump_args
def widgetExists(name):
    return clickeroo.widgetExists(name)

@dump_args
def moveToWidget(name):
    if(widgetExists(name)):
        moveToPx(clickeroo.widgetCenterX(name), clickeroo.widgetCenterY(name))

@dump_args
def clickWidget(name):
    if(widgetExists(name)):
        clickPointPx(clickeroo.widgetCenterX(name), clickeroo.widgetCenterY(name))

@dump_args
def sleep(sec):
    total = 0
    part = sec / 10.0
    while total < sec:
        total += part
        time.sleep(part)
        processEvents()

@dump_args
def testArea(x1, y1, x2, y2, r, g, b, diff):
    return clickeroo.testArea(x1, y1, x2, y2, r, g, b, diff)

@dump_args
def errorBeep(frequency = 500, duration = 1000):
    winsound.Beep(frequency, duration)
    processEvents()

@dump_args
def speak(what):
    engine = pyttsx3.init()
    engine.say(what)
    engine.runAndWait()
    processEvents()

def log(what):
    clickeroo.log(what)
    processEvents()