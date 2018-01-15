#/usr/local/bin/python
import serial
from serial import SerialException
import os
import time
import subprocess
import random
import sys


class Power:
    def __init__(self, baudRate):#busca el puerto donde esta conectado el arduino
        ttyList=subprocess.check_output('ls /dev/tty*', shell=True)#enlista todos los puertos disponibles
        time.sleep(2)
        portList=ttyList.split('\n')
        print(portList[0])
        exitFlag=False
        succedCon=False
        print(len(portList))
        portNumber=0
        self.power=0
        self.responseServer=None

        while True:
            try:
                print "abriendo ", portList[portNumber]
                self.ser = serial.Serial(portList[portNumber],baudRate, timeout=.5)
                exitFlag=True
            except SerialException:
                exitFlag=False
            if(exitFlag):#Se abrio
                for i in range(7):#intenta leer n veces
                    #print "puede ser el puerto\n lee: "
                    read_serial=self.ser.readline()
                    print read_serial
                    if len(read_serial)>1:
                        if(read_serial[0]=='o' and read_serial[1]=='k'):
                            print "dispositivo encontrado en "
                            self.port=portList[portNumber]
                            self.ser.timeout=None#con esto se queda esperando el rpi hasta que llege un dato
                            time.sleep(1)
                            print("sale con ")
                            print portList[portNumber]
                            #le contesta al micro
                            self.ser.write(b'ok')
                            self.ser.flush()
                            time.sleep(2)
                            succedCon=True
                            break
                        else:
                        #ser.close()
                            succedCon=False
                if succedCon==True:
                     break
                else:
                    self.ser.close()
            if(portNumber==(len(portList)-1)):#sigue intentando con los demas puertos
                print "No esta conectado el dispositivo"
                ttyList=subprocess.check_output('ls /dev/tty*', shell=True)#refrescar solo al finalizar toda la lista
                portList=ttyList.split('\n')
                portNumber=0
                self.ser.close()
            else:
                portNumber+=1

        print "saliendo de la configuracion inicial"

    def getPower(self):
        read_serial=self.ser.readline()
        print "datos: ", read_serial
        dataList=read_serial.split(',')
        if dataList[0]=='pot':
            self.power=dataList[1]#esta en string
            return True
        else:
            return False

    def sendStatus(self, statusRelay):
        response2Arduino='&'+status
        self.ser.write((response2Arduino))


