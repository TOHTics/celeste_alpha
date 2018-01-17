#/usr/local/bin/python
#import Power from '/home/pi/Documents/celeste_alpha/celeste/alpha/Power.py'
import imp
import sys
import time
import datetime

print "iniciando main.py a las ", datetime.datetime.now()
arduinoCom=imp.load_source('Power', '/home/pi/Documents/celeste_alpha/celeste/alpha/Power.py')
print "arduinoCom cargado a las ", datetime.datetime.now()
httpCom=imp.load_source('httpPackage', '/home/pi/Documents/celeste_alpha/celeste/alpha/httpPackage.py')
print "httpCom cargado a las ", datetime.datetime.now()

if __name__ == "__main__":
    idDevice=sys.argv[1]
    myArduinoCom=arduinoCom.Power(38400)
    myHttpCom=httpCom.Package2Send(idDevice)
    while True:
        try:
                
            if myArduinoCom.getPower()==True:#dato valido
                consumption=myArduinoCom.power
                myHttpCom.sendPower(consumption)
                if myHttpCom.getStatusRelay()==True:#status valido -> le contesta al arduino
                    myArduinoCom.sendStatus(myHttpCom.status)
                else:
                    print "informacion devuelta por  el servidor incorrecta"
            else:
                print "dato no valido"
            time.sleep(.3)
        except:
            print "Error!"
            pass



    #arduinoCom.
