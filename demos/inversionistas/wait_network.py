import serial
from serial import SerialException
from time import sleep

print ('Trying to open port')
port = None
while 1:
    try:
        port = serial.Serial(port = "/dev/ttyUSB2",baudrate = 9600,rtscts = True,dsrdtr=True,bytesize=8,parity='N',stopbits=1)
        break
    except SerialException:
        print 'USB2 is not connected yet'
        sleep(.5)
port.flush()
print('Port opened, waiting for init')


port.close()
