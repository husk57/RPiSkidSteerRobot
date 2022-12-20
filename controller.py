import requests
import pygame
import time

# requests.post("http://192.168.4.1:8080", data="hi", headers = {'Content-Type': 'plain/text'})

pygame.init()

joystick = pygame.joystick.Joystick(0)
joystick.init()

speed = 0
turn = 0
trim = 0.2
try:
    while True:
        time.sleep(1/20)
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.JOYAXISMOTION:
                id = event.axis
                val = joystick.get_axis(id)
                val = round(10*val)/10
                if (abs(val) < trim):
                    val = 0
                if (id==1):
                    speed = -val
                elif (id==2):
                    turn = val

            elif event.type == pygame.JOYBUTTONUP:
                print("Button Released")
        requests.post("http://192.168.4.1:8080", json={
            "speed": speed,
            "turn": turn
        })

except KeyboardInterrupt:
    print("EXITING NOW")
    joystick.quit()