import requests
import datetime

class Package2Send:
    def __init__(self, idDevice_):
        self.urlPost='http://f4aeeef3.ngrok.io/celeste/logger/upload/verbose'
        #self.urlPost='http://192.168.100.115:10000/celeste/logger/upload/verbose'
        self.urlGet='http://f4aeeef3.ngrok.io/celeste/device/status/arduino'
        #self.urlGet='http://192.168.100.115:10000/celeste/device/status/arduino'
        self.idDevice=idDevice_
        self.headers={'Content-Type': 'application/xml'}
        self.payloadPost=""
        self.payloadGet='{\"DeviceId\": \"' + self.idDevice + '\"}'
        self.xml=""
        self.status=None

    def sendPower(self, power_):
        self.payloadPost=self.createXml(power_)
        print "enviar con post ", self.payloadPost
        r = requests.post(self.urlPost, data=self.payloadPost, headers=self.headers)
        print 'Response content post:'
        print(r.content)
        print 'Status code: %d' % (r.status_code)

    def getStatusRelay(self):
        payload='{\"DeviceId\": \"' + self.idDevice + '\"}'
        print "enviar con get: ", payload
        r = requests.get(self.urlGet, data=self.payloadGet, headers=self.headers)
        self.status=r.content
        print 'Response get: ', r.content
        self.status="1"
        if self.status=="0" or self.status=="1":
            return True
        else:
            return False


    def createXml(self, currentPower):
        currentTime=str(datetime.datetime.utcnow()) 
        xml = '''
        <SunSpecData v="1.0">
        <d id=\"'''+self.idDevice+'''\" t=\"''' + currentTime +  '''\">
        <m id="potenciometro" x="0">
        <p id="consumo" t=\"''' + currentTime + '''\">''' + str(currentPower) + '''</p>
        </m>
        </d>
        </SunSpecData>
        '''
        return xml
