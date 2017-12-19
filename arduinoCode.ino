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

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
