#include <QueueArray.h>
#include <powerData.h>
#include <TimerOne.h>

#include <EmonLib.h>
#include <SoftwareSerial.h>
EnergyMonitor emon1;
#define emonTxV3
#define HOUR 1200

//SoftwareSerial mySerial2(11, 12); // RX, TX}
SoftwareSerial serial(5, 6); //Rx, Tx

QueueArray<powerPackage> queue;//Datos no enviados

int count3sec=0;
float powerAcum=0;
float kwH=0;
//float not
//struct 
powerPackage currentPackage;
const int pinRel=  10;     
//powerPackage currentPackage2;
int iTemp=0;
void setup() {
  // put your setup code here, to run once:
  pinMode(pinRel, OUTPUT);
  digitalWrite(pinRel, LOW);//Se activa en bajo el relevador
  serial.begin(9600);
  delay(7000);
  serial.println("iniciando");
  /*for(;;)
  {
    serial.println("iniciando");
    delay(200);    
  }*/

  emon1.current(0, 179.0);
  emon1.voltage(1, 137.0, 1.7);  // Voltage: input pin, calibration, phase_shift
  serial.print("sensores configurados: ");
  settleSensors();//Necesario por el filtro
  Timer1.initialize(3000000);         // initialize timer1, and set a 5 seconds period
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
}

void loop() 
{
  
  if(count3sec==3)//1 hora=720 de estos si el timer es de 5s
  {
    Timer1.detachInterrupt();
    kwH=powerAcum/3;
    count3sec=0;//Calcula kw hora
    powerAcum=0;
    //serial.print("kw/h: ");
    //serial.println(kwH);
    Timer1.attachInterrupt(callback); 
    //Se envían los datos
    String temp="13:16, 2017-12-14";
    currentPackage.setVals(&kwH, &temp[0]);
    //Serial.println(currentPackage.captureTime);
    //if (sentFlag==0)
    //sino se envía guarda en la cola
    if(queue.isFull()==0)//Si no está llena la cola guarda
      queue.enqueue(currentPackage);
    
  }

  if(queue.isEmpty()==0 && count3sec%20==0 )//Cada minuto intenta reenviar la cola a menos que no haya datos
  {//Intenta enviar solo uno para que pueda atender las interrupciones del timer
    //Desencola
    currentPackage.copyPackage(&(queue.dequeue()));
    //serial.println("Desencolado: ");
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
