## CyByte ##

CyByte is a +27dBm miniature variant of CyberEye's LoRa transceiver module.

The module can be used either as a LoRa transceiver module or an end device.

#### As Transceiver Module ####
The .bin files provided in modem-mode folder can be used to communicate with the module using mac commands via HOST PC or via USB-TTL Converter.
Note: Above bin files also works with CyByte EVK.

#### As a End Device ####
Projects provided in the programmable-mode folder allows 

* Basic transmission functionality and 
* Also read, write, sleep functionality of flash memory

Note: Above codes also works with CyByte EVK.

#### CyByte Evaluation Kit ####
Projects provided in the cybyte-evk folder allows us to work different sensors available on EVK.

* LDR - Detects light intensity and prints the value
* Battery Guage - Gives battery percentage at a given moment
* Basic Transmission Button - Uplink transmission happens only when the user button is pressed.