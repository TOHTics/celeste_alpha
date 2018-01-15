#ifndef powerData_h
#define powerData_h

#include <inttypes.h>
#include <string.h>
#include "Arduino.h"
#include <SoftwareSerial.h>

#define CURRENT_PIN_1 0
#define VOLTAGE_PIN_1 1
//byte CT_count;
//boolean CT1, CT2, ACAC;

class powerPackage
{
  public:
  float kwH;
  char captureTime[20];
  powerPackage();
  void setVals (float *kwH_, char* ptrTime);
  //void printVals(SoftwareSerial *ptrSerial);
  void copyPackage(powerPackage*);
};

class Sensors
{
  public:
    bool ct1;
    bool ct2;
    bool acac;
    float vrms;
    Sensors();
    void CT_Detect();
    float calc_rms(int pin, int samples);//Calcula voltaje rms 
};



#endif
