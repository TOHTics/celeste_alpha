# Celeste hardware alpha

The electronics to measure the power consumption of the users it is integrated by an arduino microcontroller and a RPI 3.
The arduino collec the sensor data: voltage (DA-10-09 transformer) and current (SCT 013), do the calculations and converts the sensors data to real power.
The sensor data is sent to the RPI trhough serial communication (UART) and the information is sent to the cloud with a 4G module (SIM7100A)

# Building
-In order to use the run the arduinoCode.ino (Aruino micro) you have to use the Arduino IDE

-In the rpi 2 case you just run seria_test.py as any python code:
````
git clone https://github.com/TOHTics/celeste_alpha.git
cd demoInver
python seria_test.py
````
