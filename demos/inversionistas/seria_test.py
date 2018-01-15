#/usr/local/bin/python
import serial
from serial import SerialException
import os
import time
import subprocess
import requests
import datetime
import random
import sys


idDevice=sys.argv[1]

if idDevice =="666":#Resetear
    ser = serial.Serial("/dev/ttyACM0",9600, timeout=.5)
    print "reiniciando..."
    time.sleep(.5)
    #ser.write(b'$')
    response2Arduino='&'+'1'
    ser.write((response2Arduino))
    ser.close()
    exit(0)
def sendPower(power):
    currentTime=str(datetime.datetime.utcnow())
    payload="""
    <sunspecdata v="1.0">
    <d id="1234" t=\" """ + currentTime + """ \">
    <m id="power" x="1">
    <p id="watts" t=\" """ + currentTime + """ \">""" + str(power) + """</p>
    </m>
    </d>
    </sunspecdata>"""

    payload = '''
    <SunSpecData v="1.0">
    <d id=\"'''+idDevice+'''\" t=\"''' + currentTime +  '''\">
    <m id="potenciometro" x="0">
    <p id="consumo" t=\"''' + currentTime + '''\">''' + str(power) + '''</p>
    </m>
    </d>
    </SunSpecData>
    '''

    print(payload)
    headers = {'Content-Type': 'application/xml'} # set what your server accepts
    #r = requests.post('http://httpbin.org/post', data=payload, headers=headers).text
    r = requests.post('http://f171b4b1.ngrok.io/celeste/logger/upload/verbose', data=payload, headers=headers)
    print 'Response content post:'
    #ser.close()
    print(r.content)
    print 'Status code: %d' % (r.status_code)

    payload='{\"DeviceId\": \"' + idDevice + '\"}'
    r = requests.get('http://f171b4b1.ngrok.io/celeste/device/status/arduino', data=payload, headers=headers)
    status=r.content
    print 'Response get: ', r.content
    return status
    #time.sleep(5)


ttyList=subprocess.check_output('ls /dev/tty*', shell=True)
print ("iniciando...")
time.sleep(2)
#print(ttyList)
portList=ttyList.split('\n')
print(portList[0])
exitFlag=False
succedCon=False
print(len(portList))
portNumber=0
while True:
    #print "intentando con: "
    #print portList[portNumber]
    try:
        ser = serial.Serial(portList[portNumber],9600, timeout=.5)
        exitFlag=True
    except SerialException:
        #print ("no es el puerto correcto!!!")
        exitFlag=False
    if(exitFlag):#Se abrio
        for i in range(5):#intenta leer n veces
            #print "puede ser el puerto\n lee: "
	    read_serial=ser.readline()
            print read_serial
            if len(read_serial)>1:
                if(read_serial[0]=='o' and read_serial[1]=='k'):
                    print "dispositivo encontrado"
                    ser.timeout=None
                    time.sleep(1)
                    print("sale con ")
                    print portList[portNumber]
                    #le contesta al micro
                    ser.write(b'ok')
                    ser.flush()
                    time.sleep(2)
                    succedCon=True
                    break
                else:
                #ser.close()
                    succedCon=False
        if succedCon==True:
             break
        else:
            ser.close()
    if(portNumber==(len(portList)-1)):#sigue intentando con los demas puertos
        print "No esta conectado el dispositivo"
        ttyList=subprocess.check_output('ls /dev/tty*', shell=True)#refrescar solo al finalizar toda la lista
        portList=ttyList.split('\n')
        portNumber=0
        ser.close()
    else:
        portNumber+=1

print "saliendo de la configuracion inicial"

s = [0,1]

while True:
	read_serial=ser.readline()
	#s[0] = str(int (ser.readline(),16))
        print ("datos: ", read_serial)
        dataList=read_serial.split(',')
        if dataList[0] == 'pot':
            status=sendPower(dataList[1])#Analizar la respuesta del servidor de Carlos sino envia lo que quiero no enviar nada al arduino
            response2Arduino='&'+status
            ser.write((response2Arduino))
            #A001-3312-312B


            #print "data = "
	    #print dataList[1]


