import requests
import serial

ser = serial.Serial('/dev/ttyS1', 9600)

# line_notify_token = 'aJ7CmvccUKpDtLAbwyjTuGYlAbdqtEgSQxM2XzgErqD'
line_notify_token = '2P3gmeMzTlxWo31cwdmxsjRqHAYJDyoLjNkLuCQrIJ3'

line_notify_api = 'https://notify-api.line.me/api/notify'

def lineNotify(message):
    payload = {'message':message}
    headers = {'Authorization':'Bearer '+line_notify_token}    
    return requests.post(line_notify_api, headers=headers, data = payload)

olddata = None
while(True):
    data = ser.readline().decode().strip()
    print("recieve :",data)
    if(data == "0" or data == "1" or data == "2" or data == "3" or data == "4" or data == "5"): 
        #mode = ["timer LED", "Light Sensor", "Moving Sensor"]
        text = 'your score is ' + data
        lineNotify(text)
    elif (data == "6" or data == "7" or data == "8" or data == "9" or data == "10"):
        text = 'your score is ' + data
        lineNotify(text)
    elif(data == "11" or data == "12" or data == "13" or data == "14" or data == "15"):
        text = 'your score is ' + data
        lineNotify(text)
    elif(data == "16" or data == "17" or data == "18" or data == "19" or data == "20"):
        text = 'your score is ' + data
        lineNotify(text)
    #elif((data == "LED is OFF" or data == "LED is ON") and (olddata != data)):
        #olddata = data
        #lineNotify(data)