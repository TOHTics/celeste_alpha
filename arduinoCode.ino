#include <LowPower.h>

#include <MySIM800L.h>
#include <QueueArray.h>
#include <powerData.h>
#include <TimerOne.h>
#include <EmonLib.h>
//#include <SoftwareSerial.h>
EnergyMonitor emon1;
Sensors sensors;

#define emonTxV3
#define SAMPLES_HOUR 5 //1 hora=720 de estos si el timer es de 5s
#define MICRO_HOUR 1000000
//SoftwareSerial mySerial2(11, 12); // RX, TX}
SoftwareSerial serial(5, 6); //Rx, Tx
MySIM800L gsm(11, 12);

QueueArray<powerPackage> queue;//Datos no enviados
//MySIM800L gsm(11, 12);

int count10sec=0;
float powerAcum=0;
float kwH=0;
//float not
//struct 
powerPackage currentPackage;
const byte pinRel=  10;     
bool timeOutFlag=false;
//powerPackage currentPackage2;
//int iTemp=0;
void setup() {
  // put your setup code here, to run once:
  analogReference(EXTERNAL);
  pinMode(pinRel, OUTPUT);
  digitalWrite(pinRel, LOW);//Se activa en bajo el relevador
  serial.begin(74880);
  delay(5000);
  serial.println("iniciando");
  for(;;)
  {

    sensors.CT_Detect();//Detecta los sensores de corriente y el adaptador de ac-ac
    if(sensors.acac)
    {
      serial.println("sensor conectado");
      break;
    }
    else
      serial.println("no hay sensor conectado");    
  }
  //
  /*for(;;)
  {
    serial.println("iniciando");
    delay(200);    
  }*/
  
  /*if(gsm.init(9600)) serial.println("Ready to use GSM");
  if(gsm.signal()) serial.println("With Signal");
  gsm.setAPN("internet.itelcel.com","webgprs", "webgprs2003");
  if(gsm.connectGSM()) serial.println("GSM Ready");
  if(gsm.connectHTTP()) serial.println("HTTP API Ready");
  String data = createXML("4001", "40", "01-12-2017");
  //serial.println(gsm.http_method("0","http://9869d310.ngrok.io/celeste/device/status","application/json",{"DeviceId": "4001"}));
  serial.println(gsm.http_method("1","http://0d68a09a.ngrok.io","application/xml",data));//1 post
*/

  emon1.current(CURRENT_PIN_1, 112);//Con 179  funciona bien, probar con 90.9
  emon1.voltage(VOLTAGE_PIN_1, 90.35, 1.7);  // Voltage: input pin, calibration, phase_shift
  /*pinMode(A0, OUTPUT);
  analogWrite(A0, 255);
  
  analogWrite(A0, 0);     // Pull input high low high then low to disburse any residual charge to avoid incorrectly detecting AC wave when not present
  analogWrite(A0, 255);
  analogWrite(A0, 0);     
  analogWrite(A0, 255);
  analogWrite(A0, 0);     
  analogWrite(A0, 255);
  analogWrite(A0, 0);     
  pinMode(A0,INPUT);*/
  delay(500);             //allow things to settle

  serial.print("sensores configurados: ");
  settleSensors();//Necesario por el filtro
  Timer1.initialize(5*MICRO_HOUR);
  //Timer1.initialize(5000000);         // initialize timer1, and set a 5 seconds period, originalmente esta función está en us
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
 // mySim.connectGSM();
}

String createXML(String id, String potencia, String date)
{
  return "<SunSpecData v=\"1.0\">\n<d id=\""+id+"\" t=\""+date+"\">\n<m id=\"potenciometro\" x=\"1\">\n<p id=\"potencia\" t=\""+date+"\">"+potencia+"</p>\n</m>\n</d>\n</SunSpecData>";
}

void loop() 
{
  
  if(count10sec==SAMPLES_HOUR)//1 hora=720 de estos si el timer es de 5s
  {
    Timer1.detachInterrupt();
    kwH=powerAcum/SAMPLES_HOUR;
    count10sec=0;//Calcula kw hora
    powerAcum=0;
    //serial.print("kw/h: ");
    //serial.println(kwH);
    Timer1.attachInterrupt(callback); 
    //Se envían los datos, implementar funciones de envío aqui
    String temp="13:16, 2017-12-14";
    currentPackage.setVals(&kwH, &temp[0]);
    serial.println("W/H = ");
    serial.println(currentPackage.kwH);
    //if (sentFlag==0)
    //sino se envía guarda en la cola
    if(queue.isFull()==0)//Si no esta llena la cola guarda
    {
      serial.println("Aún no está llena");
      queue.enqueue(currentPackage);
    }
    else
    {
      serial.print("Cola llena con ");
      serial.println(queue.count());

      
    }

  }
    /*serial.print("count10sec: ");
    serial.println(count10sec);
    serial.print("modulo: ");
    serial.println(count10sec%5);//Prueba cada 6*/
  
    if(queue.isEmpty()==0 && count10sec%2==0 )//Cada minuto intenta reenviar la cola a menos que no haya datos
    {//Intenta enviar solo uno para que pueda atender las interrupciones del timer
      //Desencola
      // currentPackage.copyPackage(&(queue.dequeue()));
      //serial.println("Desencolando: ");
      //serial.println(currentPackage.kwH);
      //Intenta enviar
      
    }
  
 // LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
delay(10);
}

void callback()
{
    /*for(;;)
    {
  
      sensors.CT_Detect();//Detecta los sensores de corriente y el adaptador de ac-ac
      if(sensors.acac)
        break;
      
      else
        serial.println("no hay sensor conectado"); 
      delay(10);   
    }*/
    // put your main code here, to run repeatedly:
  emon1.calcVI(80, 2000, &timeOutFlag);        // Calculate all. No.of wavelengths, time-out, originalmente 20 y 2000
  //En 35 muestras está probado funcionando oc
  //emon1.serialprint();           // Print out all variables
     count10sec++;
     if(timeOutFlag)
     {
       serial.println("Timeout! ");
     }
   serial.println("Potencia real= ");
   serial.println(emon1.realPower);

   serial.println("Vrms= ");
   serial.println(emon1.Vrms);
   serial.println("Irms= ");
   serial.println(emon1.Irms);
   powerAcum+=emon1.realPower;
   //powerAcum+=10.5+iTemp;
   //iTemp++;
}

void settleSensors()
{
  serial.print("leyendo sensores: ");
  for(int i=0; i<25; i++)
  {
    //serial.print("leyendo...");
    emon1.calcVI(25, 2000, &timeOutFlag);        
  }
  serial.print("filtro establecido: ");
}

