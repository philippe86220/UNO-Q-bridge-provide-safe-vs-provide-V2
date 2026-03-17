from arduino.app_utils import App, Bridge
import time

def loop():
    Bridge.call("print_value", 12.37)
    time.sleep(0.05)

App.run(user_loop=loop)
