## About CyberEye LoRa® Transceiver Module ##

CyberEye's LoRa® technology transceiver module is a highly integrated, ultra low power, programmable module. It is based on Microchip’s ATSAMR34 SiP which provides a number of highly configurable peripherals including I2C/SPI/UART interfaces along with a 12-bit ADC. The integration of an MCU and LoRa® transceiver within a single package of SiP reduces the form factor and minimizes the time to market.

This module is a perfect solution for any LoRaWAN® end node developer. It offers a cost-efficient and compact solution, allowing for rapid development and deployment in a wide variety of IoT scenarios that require long-range connectivity and great battery life. The different onboard connectors for antenna makes it easy to start prototyping without additional circuitry. A special feature called Persistent Data Server (PDS) in the module allows users to store frame counter and other info in the non-volatile memory of the module as required in LoRaWAN® 1.1 Specification.
 
### LoRa® Transceiver Module variants ###
 
The module comes in 2 variants.
 
1. CyBit - A +20dBm variant of the CyberEye's LoRa transceiver module which is an RN2483 drop in replacement.
 
2. CyByte - A +27dBm miniature variant of CyberEye's LoRa transceiver module with dimensions ~13mm X 21mm. The variant is specifically designed to complement the solutions that require long-range connectivity and in-turn reliability where a +20dBm LoRa module might not be sufficient. An added feature for CyByte is the on-board flash memory for FUOTA implementation. The final specification document and release samples will be shared based on the requirement. 


#### Changelog - CyByte ###

##### V2.0.0
* TFLXLoRa Secure Element functionality is added
	* CyBit: Modem Mode, Basic Transmission, User interactive LoRaWAN applications
	* CyByte: Modem Mode, Basic Transmission, User interactive LoRaWAN applications
* Kept the Flash in deep sleep in Basic Transmission Application which reduces the power consumption from ~14-17 uA to ~2.3-2.7 uA uA during the Standby Sleep Mode
* CyByte EVK Sensor codes are added.
	* LDR
	* Battery Guage
	* Basic Transmission when User Button pressed

##### V1.1.0
* Kept the Flash in deep sleep in Modem Mode application and User interactive lorawan application which reduces the power consumption from ~14-17 uA to ~2.3-2.7 uA uA during the Standby Sleep Mode
* Programmable Mode
	* Flash Memory Application which supports basic read, write functionality as well as can keep it in deep sleep mode.

##### V1.0.0
* Programmable Mode 
	* Basic Transmission Codes
* Modem Mode Application
	* Supports ASCII Command set
