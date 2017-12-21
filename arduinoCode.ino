#include <MySIM800L.h>
#include <QueueArray.h>
#include <powerData.h>
#include <TimerOne.h>
#include <EmonLib.h>
//#include <SoftwareSerial.h>
EnergyMonitor emon1;
#define emonTxV3
#define SAMPLES_HOUR 5 //1 hora=720 de estos si el timer es de 5s

//SoftwareSerial mySerial2(11, 12); // RX, TX}
SoftwareSerial serial(5, 6); //Rx, Tx
MySIM800L gsm(11, 12);


QueueArray<powerPackage> queue;//Datos no enviados
//MySIM800L gsm(11, 12);

int count3sec=0;
float powerAcum=0;
float kwH=0;
//float not
//struct 
powerPackage currentPackage;
const byte pinRel=  10;     

//powerPackage currentPackage2;
//int iTemp=0;
void setup() {
  // put your setup code here, to run once:
  pinMode(pinRel, OUTPUT);
  digitalWrite(pinRel, LOW);//Se activa en bajo el relevador
  serial.begin(9600);
  delay(1000);
  serial.println("iniciando");
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
  emon1.current(CURRENT_PIN_1, 179.0);
  emon1.voltage(VOLTAGE_PIN_1, 137.0, 1.7);  // Voltage: input pin, calibration, phase_shift
  serial.print("sensores configurados: ");
  settleSensors();//Necesario por el filtro
  Timer1.initialize(3000000);         // initialize timer1, and set a 5 seconds period
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
 // mySim.connectGSM();
}

String createXML(String id, String potencia, String date)
{
  return "<SunSpecData v=\"1.0\">\n<d id=\""+id+"\" t=\""+date+"\">\n<m id=\"potenciometro\" x=\"1\">\n<p id=\"potencia\" t=\""+date+"\">"+potencia+"</p>\n</m>\n</d>\n</SunSpecData>";
}

void loop() 
{
  
  if(count3sec==SAMPLES_HOUR)//1 hora=720 de estos si el timer es de 5s
  {
    Timer1.detachInterrupt();
    kwH=powerAcum/SAMPLES_HOUR;
    count3sec=0;//Calcula kw hora
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
    /*serial.print("count3sec: ");
    serial.println(count3sec);
    serial.print("modulo: ");
    serial.println(count3sec%5);//Prueba cada 6*/
  
    if(queue.isEmpty()==0 && count3sec%2==0 )//Cada minuto intenta reenviar la cola a menos que no haya datos
    {//Intenta enviar solo uno para que pueda atender las interrupciones del timer
      //Desencola
      // currentPackage.copyPackage(&(queue.dequeue()));
      //serial.println("Desencolando: ");
      //serial.println(currentPackage.kwH);
      //Intenta enviar
      
    }
  
  
delay(1);
}

void callback()
{
    // put your main code here, to run repeatedly:
  emon1.calcVI(20, 2000);        // Calculate all. No.of wavelengths, time-out
  //emon1.serialprint();           // Print out all variables
     count3sec++;
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
    emon1.calcVI(20, 2000);        
  }
  serial.print("filtro establecido: ");
}

