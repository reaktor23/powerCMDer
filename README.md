# powerCMDer

powerCMDer (power commander) is a power control and environment monitoring device for our Hackerspace, Reaktor23.

It is powered by MQTT, an Arduino Ethernet and a custom made shield that let us conrol the contactors for two power circuits and connection for several sensors.
The shield has a hardware fallback, so in case the Arduino dies, we're always able to control the power circuits manualy.
It is also used to determin the state of our hackerspace, e.g. closed, unknown, open.
