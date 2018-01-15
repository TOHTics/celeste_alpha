import requests
import datetime

class Package2Send:
    def __init__(self, idDevice_):
        self.urlPost='http://f171b4b1.ngrok.io/celeste/logger/upload/verbose'
        self.urlGet='http://f171b4b1.ngrok.io/celeste/device/status/arduino'
        self.idDevice=idDevice_
        self.headers={'Content-Type': 'application/xml'}
        self.payloadPost=""
        self.payloadGet='{\"DeviceId\": \"' + self.idDevice + '\"}'
        self.xml=""
        self.status=None

    def sendPower(self, power_):
        self.power=power_
        self.payloadPost=self.createXml()
        r = requests.post(self.urlPost, data=self.payloadPost, headers=self.headers)
        print 'Response content post:'
        #ser.close()
        print(r.content)
        print 'Status code: %d' % (r.status_code)

    def getStatusRelay(self):
        payload='{\"DeviceId\": \"' + self.idDevice + '\"}'
        r = requests.get(self.urlGet, data=self.payloadGet, headers=self.headers)
        self.status=r.content
        print 'Response get: ', r.content
        if self.status=="0" or self.status=="1":
            return True
        else:
            return False


    def createXml(self):
        currentTime=str(datetime.datetime.utcnow()) 
        xml = '''
        <SunSpecData v="1.0">
        <d id=\"'''+self.idDevice+'''\" t=\"''' + currentTime +  '''\">
        <m id="potenciometro" x="0">
        <p id="consumo" t=\"''' + currentTime + '''\">''' + str(self.power) + '''</p>
        </m>
        </d>
        </SunSpecData>
        '''
        return xml
