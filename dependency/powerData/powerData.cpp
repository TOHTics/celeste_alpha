#include "powerData.h"



powerPackage::powerPackage()
{
  kwH=0;
}

void powerPackage::setVals(float *kwH_,char* ptrTime)
{
  kwH=*kwH_;
  strcpy(captureTime, ptrTime);

}
void powerPackage::copyPackage(powerPackage* ptrPackage)
{
  kwH=ptrPackage->kwH;
  strcpy(captureTime, ptrPackage->captureTime);
}

Sensors::Sensors()
{
  ct1=false;
  ct2=false;
  acac=false;
}


void Sensors::CT_Detect()
{
//--------------------------------------------------Check for connected CT sensors---------------------------------------------------------------------------------------------------------
  //Es necesario un conector hembra especial para esto:
//if (analogRead(CURRENT_PIN_1) > 0) {ct1 = true; } else ct1=false; // check to see if CT is connected to CT1 input, if so enable that channel
//if (analogRead(2) > 0) {CT2 = 1; CT_count++;} else CT2=0;  // check to see if CT is connected to CT2 input, if so enable that channel
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------Check for connected AC Adapter Sensor------------------------------------------------------------------------------------------------

// Quick check to see if there is a voltage waveform present on the ACAC Voltage input
// Check consists of calculating the RMS from 100 samples of the voltage input.
//delay(5000); // Unlike on the emonTx V3 since the emonPi is not powered by AC-DC circuit there should not be need for delay to allow residual AC in resivour capacitot to diminnish.
//digitalWrite(LEDpin,LOW);

pinMode(VOLTAGE_PIN_1, OUTPUT);
analogWrite(VOLTAGE_PIN_1, 255);
analogWrite(VOLTAGE_PIN_1, 0);     // Pull input high low high then low to disburse any residual charge to avoid incorrectly detecting AC wave when not present
pinMode(VOLTAGE_PIN_1,INPUT);
delay(500);             //allow things to settle

// Calculate if there is an ACAC adapter on analog input 0
//vrms = calc_rms(VOLTAGE_PIN_1, 1780) * 0.87;      //ADC 0   double vrms = calc_rms(0,1780) * (Vcal * (3.3/1024) );
vrms = calc_rms(VOLTAGE_PIN_1, 1000) * 0.87;      //ADC 0   double vrms = calc_rms(0,1780) * (Vcal * (3.3/1024) );
if (vrms>90) acac = true; else acac=false;
//Serial.print(vrms);

}
/*void printVals(SoftwareSerial *ptrSerial)
{
  int a =0;
}*/

float Sensors::calc_rms(int pin, int samples)
{
  unsigned long sum = 0;
  for (int i=0; i<samples; i++)                           // 178 samples takes about 20ms
  {
    int raw = (analogRead(pin)-512);
    sum += (unsigned long)raw * raw;
  }
  double rms = sqrt((double)sum / samples);
  return rms;
}
