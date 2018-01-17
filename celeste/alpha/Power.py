#/usr/local/bin/python
import serial
from serial import SerialException
import os
import time
import subprocess
import random
import sys
import datetime


class Power:
    def __init__(self, baudRate):#busca el puerto donde esta conectado el arduino
        print "iniciando Power init a las ", datetime.datetime.now()
        ttyList=subprocess.check_output('ls /dev/ttyACM*', shell=True)#enlista todos los puertos disponibles
        time.sleep(2.5)
        portList=ttyList.split('\n')
        print(portList[0])
        exitFlag=False
        succedCon=False
        print(len(portList))
        portNumber=0
        self.power=0
        self.responseServer=None
        self.statusSerial=False#Conectado al arduino
        self.br=baudRate
        print "iniciando la apertura de puertos a las ", datetime.datetime.now()

        while True:
            try:
                print "abriendo ", portList[portNumber]
                self.ser = serial.Serial(portList[portNumber],self.br, timeout=.5)
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
                            print "dispositivo encontrado en ", portList[portNumber]
                            self.port=portList[portNumber]
                            self.ser.timeout=None#con esto se queda esperando el rpi hasta que llege un dato
                            print "dispositivo conectado a las ", datetime.datetime.now()
                            time.sleep(1)
                            #le contesta al micro
                            self.ser.write(b'ok')
                            self.ser.flush()
                            time.sleep(.6)
                            succedCon=True
                            self.statusSerial=True
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
        if self.statusSerial==True:
            try:
                read_serial=self.ser.readline()
            except SerialException:
                print "Desconexion del puerto serial, reconectar!"
                self.statusSerial=False
                self.ser.close()
                return False
        else:
            print "Intentando reabrir el puerto ", self.port
            try:
                self.ser = serial.Serial(self.port, self.br)
                read_serial=self.ser.readline()
                print "se abrio y el dato: ", read_serial
                self.statusSerial=True
            except SerialException:
                time.sleep(.65)
                self.statusSerial=False
                self.ser.close()
                return False

        print "datos: ", read_serial
        dataList=read_serial.split(',')
        if dataList[0]=='pot':
            self.power=dataList[1]#esta en string
            return True
        if dataList[0]=='ok':
            #time.sleep(.6)
            #le contesta al micro
            self.ser.write(b'ok')#para que salga de su status de waiting
            self.ser.flush()
            time.sleep(.2)
            return False
        else:
            return False

    def sendStatus(self, statusRelay):
        response2Arduino='&'+statusRelay
        self.ser.write((response2Arduino))


