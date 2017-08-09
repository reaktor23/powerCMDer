# powerCMDer

powerCMDer (power commander) is a power control and environment monitoring device for our Hackerspace, Reaktor23.

It is powered by MQTT, an Arduino Ethernet and a custom made shield that let us conrol the contactors for two power circuits and connection for several sensors.
The shield has a hardware fallback, so in case the Arduino dies, we're always able to control the power circuits manualy.
It is also used to determin the state of our hackerspace, e.g. closed, unknown, open.

![layout](https://raw.githubusercontent.com/reaktor23/powerCMDer/master/hardware/powerCMDer-layout.png)

## MQTT topics

 - powerCMDer/temperature - publishes the actual room temperature
 - powerCMDer/powermeter/tick - publishes whenever a tick is received from the powermeter (800 pulses per kWh)
 - powerCMDer/outlets/status - publishes the current status of the circuit, either 1 or 0
 - powerCMDer/outlets - subscription for controling the circuit, payload is either 1 or 0
 - powerCMDer/server/status - publishes the current status of the circuit, either 1 or 0
 - powerCMDer/server - subscription for controling the circuit, payload is either 1 or 0
 
