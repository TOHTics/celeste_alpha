#include <SoftwareSerial.h>
SoftwareSerial serial(5, 6); //Rx, Tx
#include <QueueArray.h>
#include <powerData.h>
#include <TimerOne.h>
#include <EmonLib.h>

#define emonTxV3
#define SAMPLES_HOUR 5 //1 hora=720 de estos si el timer es de 5s
#define MICRO_HOUR 1000000
EnergyMonitor emon1;
Sensors sensors;
QueueArray<powerPackage> queue;//Datos no enviados

int count10sec=0;
float powerAcum=0;
float kwH=0;
//float not
//struct 
powerPackage currentPackage;
const byte pinRel=  10;     
bool timeOutFlag=false;


char buf[3];
void setup() {
    serial.begin(9600);//Debugear
    Serial.begin(9600);//Conectado al rpi
  delay(1000);
  serial.println("Iniciando...");
  startRpi();

  analogReference(EXTERNAL);//Hay que cambiar la referencia porque origialmente funciona con 5v
  pinMode(pinRel, OUTPUT);
  digitalWrite(pinRel, LOW);//Se activa en bajo el relevador
  delay(1000);
  serial.println("iniciando módulos...");
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

  emon1.current(CURRENT_PIN_1, 112);//Con 179  funciona bien, probar con 90.9
  emon1.voltage(VOLTAGE_PIN_1, 90.35, 1.7);  // Voltage: input pin, calibration, phase_shift

  delay(500);             //allow things to settle

  serial.print("sensores configurados: ");
  settleSensors();//Necesario por el filtro
  
  Timer1.initialize(5*MICRO_HOUR);
  //Timer1.initialize(5000000);         // initialize timer1, and set a 5 seconds period, originalmente esta función está en us
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt

}

void(* resetFunc) (void) = 0;//declare reset function at address 0

void loop() {

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

  
//Serial.println("pot,156.9,");

    if(Serial.available()>0)//Lega un dato
    {
      buf[0]=Serial.read();
      if(buf[0]=='$')//Resetear
      {
        serial.println("El micro se reiniciará");
        resetFunc();
      }
      if (buf[0]=='&')
      {
        
        buf[1]=Serial.read();
        serial.println("cambiar de estado");
        serial.println("llegó: "+buf[1]);
        if(buf[1]=='1')//Encendido
        {
          serial.println("Encender");
          digitalWrite(pinRel, LOW);
        }
        if(buf[1]=='0')
        {
          serial.println("Apagar");
          digitalWrite(pinRel, HIGH);
        }
        
      }
      
    }

  delay(1);
  
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
   Serial.println("pot,"+String(emon1.realPower)+",");
   //powerAcum+=10.5+iTemp;
   //iTemp++;
}

void settleSensors()
{
  serial.print("leyendo sensores: ");
  for(int i=0; i<25; i++)
  {
    //serial.print("leyendo...");
    emon1.calcVI(20, 2000, &timeOutFlag);        
  }
  serial.println("filtro establecido: ");
}

void startRpi()
{

  for(;;)
  {
    Serial.println("ok");
    //Serial.println("enviando ocs ");
    //int temp=serial.available();
    //Serial(String(temp));
    if(Serial.available()>0)
    {
      serial.println("if");
      buf[0]=Serial.read();
      buf[1]=Serial.read();
      if(buf[0]=='o' && buf[1]=='k')
      {
        serial.println("rpi listo!");//Arduino y rpi sincronizados
        break;
      } 
    }
    delay(100);
  }
  
}

