from http import client
from pydoc import cli
import serial
import paho.mqtt.client as mqtt
import json
import requests

client = mqtt.Client()

mqttServerHost = "202.168.255.20"
mqttServerPort = 1883
mqttUserName = "bnmqtt-user"
mqttUserPass = "w7tfg6wtg"
mqttKeepAlive = 60

def on_connect(client, userdata, flags, rc):
    print("Result from connect: {}".format(mqtt.connack_string(rc)))


client.username_pw_set(username=mqttUserName, password=mqttUserPass)


ser = serial.Serial('/dev/ttyACM0', 9600)
url = "https://google.com"
timeout = 5
internet_status = False

def check_internet_status():
    try:
        print("Checking internet Connection please wait: ")
        request = requests.get(url, timeout=timeout)
        if request.status_code == 200:
            print("Connected to the Internet")
            internet_status = True
            print("Connecting...")
            client.connect(mqttServerHost, mqttServerPort, mqttKeepAlive)
            return internet_status
    except (requests.ConnectionError, requests.Timeout) as exception:
        print("No internet connection.")



while True:
    internet_status = check_internet_status()
    if not internet_status:
        check_internet_status()
    else:
        ser.flushInput()
        incomingRawMsg = ser.readline().decode("utf-8")
        print(incomingRawMsg)
        try:           
            incomingMsgToJson = json.loads(incomingRawMsg)
            receiveTemperature = incomingMsgToJson["temp"]
            receiveHumidity = incomingMsgToJson["hum"]
            receiveRSSI = incomingMsgToJson["rssi"]
            print("Receive from Serial: {}".format(incomingMsgToJson))
            print("Published")
            client.publish("lora/temp", receiveTemperature)
            client.publish("lora/hum", receiveHumidity)
            client.publish("lora/rssi", receiveRSSI)
        except:
            print("Message format error!!!")
    
    

    


