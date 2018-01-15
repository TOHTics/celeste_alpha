#/usr/local/bin/python
#import Power from '/home/pi/Documents/celeste_alpha/celeste/alpha/Power.py'
import imp
import sys

arduinoCom=imp.load_source('Power', '/home/pi/Documents/celeste_alpha/celeste/alpha/Power.py')
httpCom=imp.load_source('httpPackage', '/home/pi/Documents/celeste_alpha/celeste/alpha/httpPackage.py')

if __name__ == "__main__":
    idDevice=sys.argv[1]
    myArduinoCom=arduinoCom.Power(9600)
    myHttpCom=httpCom.Package2Send(idDevice)
    while True:
        if myArduinoCom.getPower()==True:#dato valido
            consumption=myArduinoCom.power
            myHttpCom.sendPower(consumption)
            if myHttpCom.getStatusRelay()==True:#status valido -> le contesta al arduino
                myArduinoCom.sendStatus(myHttpCom.status)
            else:
                print "informacion devuelta por  el servidor incorrecta"
        else:
            print "dato no valido"



    #arduinoCom.
