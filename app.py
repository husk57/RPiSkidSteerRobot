from flask import Flask
app = Flask(__name__)
from flask import render_template
from flask import request
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)
#software pwm
rightDriverList = [7, 9, 14, 11, 16, 13,15] #power, ground, ground, right enable, left enable, rpwm, lpwm
GPIO.setup(rightDriverList[0], GPIO.OUT)
GPIO.output(rightDriverList[0], GPIO.HIGH)

GPIO.setup(rightDriverList[3], GPIO.OUT)
GPIO.output(rightDriverList[3], GPIO.HIGH)

GPIO.setup(rightDriverList[4], GPIO.OUT)
GPIO.output(rightDriverList[4], GPIO.HIGH)

GPIO.setup(rightDriverList[5], GPIO.OUT)
GPIO.setup(rightDriverList[6], GPIO.OUT)

rightPwmR = GPIO.PWM(rightDriverList[5], 100)
rightPwmR.start(0)

rightPwmL = GPIO.PWM(rightDriverList[6], 100) # never wants to run
rightPwmL.start(0)

#software pwm
leftDriverList = [18, 20, 22, 29, 31, 36] #power, ground, right enable, left enable, rpwm, lpwm

GPIO.setup(leftDriverList[0], GPIO.OUT)
GPIO.output(leftDriverList[0], GPIO.HIGH)

GPIO.setup(leftDriverList[2], GPIO.OUT)
GPIO.output(leftDriverList[2], GPIO.HIGH)

GPIO.setup(leftDriverList[3], GPIO.OUT)
GPIO.output(leftDriverList[3], GPIO.HIGH)

GPIO.setup(leftDriverList[4], GPIO.OUT)
GPIO.setup(leftDriverList[5], GPIO.OUT)

leftPwmR = GPIO.PWM(leftDriverList[4], 100)
leftPwmR.start(0)

leftPwmL = GPIO.PWM(leftDriverList[5], 100)
leftPwmL.start(0)

cutoff = 30

def setRightMotorSpeed(percentOutput):
    if (abs(percentOutput) > 100):
        return
    if (abs(percentOutput) < cutoff):
        rightPwmR.ChangeDutyCycle(0)
        rightPwmL.ChangeDutyCycle(0)
        return
    if (percentOutput > 0):
        rightPwmR.ChangeDutyCycle(0)
        rightPwmL.ChangeDutyCycle(percentOutput)
    else:
        rightPwmL.ChangeDutyCycle(0)
        rightPwmR.ChangeDutyCycle(-1*percentOutput)

def setLeftMotorSpeed(percentOutput):
    if (abs(percentOutput) > 100):
        return
    if (abs(percentOutput) < cutoff):
        leftPwmR.ChangeDutyCycle(0)
        leftPwmL.ChangeDutyCycle(0)
        return
    if (percentOutput > 0):
        leftPwmR.ChangeDutyCycle(0)
        leftPwmL.ChangeDutyCycle(percentOutput)
    else:
        leftPwmL.ChangeDutyCycle(0)
        leftPwmR.ChangeDutyCycle(-1*percentOutput)

@app.route('/', methods = ['GET', 'POST'])
def hello_world():
    dataToReturn = None
    if request.method == 'GET':
        dataToReturn = render_template('index.html')
    else:
        data = request.get_json()
        throttle = int(data['throttle'])
        turning = int(data['turn'])
        print(throttle, turning)
        setLeftMotorSpeed(throttle*.25)

        dataToReturn = "Ok."
    return dataToReturn
if __name__ == '__main__':
    app.run(host='0.0.0.0')