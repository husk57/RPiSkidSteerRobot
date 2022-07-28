from flask import Flask
from flask import render_template
from flask import request
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)
#software pwm
rightDriverList = [7, 9, 14, 11, 16, 13, 12] #power, ground, ground, right enable, left enable, rpwm, lpwm
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

rightPwmL = GPIO.PWM(rightDriverList[6], 100)
rightPwmL.start(100)

app = Flask(__name__)

@app.route('/', methods = ['GET', 'POST'])
def hello_world():
    dataToReturn = None
    if request.method == 'GET':
        dataToReturn = render_template('index.html')
    else: 
        data = request.get_json()
        print(data)
        dataToReturn = "Ok."
    return dataToReturn
if __name__ == '__main__':
    app.run(host='0.0.0.0')