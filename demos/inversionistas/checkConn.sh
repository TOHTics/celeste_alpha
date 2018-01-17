#!/bin/bash
set -e

python /home/pi/Documents/celeste_alpha/demos/inversionistas/wait_network.py > /home/pi/Documents/celeste_alpha/demos/inversionistas/outWaitUSB.log
sleep 10
#sudo ifconfig wwan0 down
#sleep 2
#sudo wvdial &
#sleep 10
python /home/pi/Documents/celeste_alpha/demos/inversionistas/main.py 666 > /home/pi/Documents/celeste_alpha/demos/inversionistas/out.log
#sleep 20

#(
#	while : ; do
#		sudo ifconfig wwan0 down
#		sleep 1
#		sudo wvdial
#		sleep 10
#	done
#) &
#python TaxiClient.py &
