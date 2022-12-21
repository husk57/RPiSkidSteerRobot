import requests
import pygame
import time
import math
import websocket

ws = websocket.WebSocket()
ws.connect("ws://192.168.4.1:8080/ws")

pygame.init()

joystick = pygame.joystick.Joystick(0)
joystick.init()

speed = 0.69
turn = 0
trim = 0.2

try:
    while True:
        changed = False
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.JOYAXISMOTION:
                id = event.axis
                val = joystick.get_axis(id)
                val = round(10*val)/10
                if (abs(val) < trim):
                    val = 0
                if (id==1):
                    pastSpeed = speed
                    speed = -val
                    if (abs(pastSpeed-speed) > 0.001):
                        changed = True
                elif (id==2):
                    pastTurn = turn
                    turn = val
                    if (abs(pastTurn-turn) > 0.001):
                        changed = True

            elif event.type == pygame.JOYBUTTONUP:
                print("Button Released")
        if changed == True:
            print(speed, turn)
            ws.send(str(speed)+"$"+str(turn))
        time.sleep(1/20)

except KeyboardInterrupt:
    print("EXITING NOW")
    joystick.quit()