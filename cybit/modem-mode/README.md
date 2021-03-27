# Modem Mode Stack #

This readme contains the default configuration that is set in the modem mode stack of CyBit for both 14dBm, 20dBm.

## Modem Mode Command Set Capabilities: ##

* LoRaWAN Class A and C Protocol Compliant, and supports different lora parameters which can be configured
	* Activation Methods: ABP and OTAA
	* Device Class: Class A and Class C
	* Frequency Plan
		* IND865 : 8 Channels
			* 3 Default Channels (865.0625, 865.4025, 865.985)
			* 5 Extra Channels (865.2325, 866.185, 866.385, 866.585, 866.785)
		* EU868: 8 Channels
			* 	3 Default channels(868.1, 868.3, 868.5)
			* 	5 Extra Channels (867.1, 867.3, 867.5, 867.7, 867.9)
	* Uplinks: Confirmed and Unconfirmed
	* Downlinks: Confirmed and Unconfirmed
	* Data rate: DR0 - DR5	
	* Adaptive Data rate: ON and OFF		
	* Power Index: 0 - 7	
	* Adding Channels - Frequency, Status, Data Rate Range
	* RX2 Window Parameters such as datarate, frequency.
	* Linkcheck period
	* Device Battery
	* Retransmissions count for Confirmed Uplinks
	* RXDelays: RX1
	* etc.
* Intelligent Low-Power mode with programmable wake-up(after a specified time) and consumes ~8.4 - ~8.9uA in standby mode.
* All configuration and control have done over UART using simple ASCII commands with a baud rate of 115200
* PDS to store the LoRaWAN Parameters and join configuration using mac save and restore even after power-on reset or sys reset.
* Configurable Secure Join(HSM): Implemented


## Default Configuration of the stack ##

* Transmission Type: Unconfirmed
* Device Class: Class A
* Data Rate: DR5
* ADR: False
* Freq Plan: IND865
* IND865 : 
	* 3 Default Channels (865.0625, 865.4025, 865.985)
	* 5 Extra Channels (865.2325, 866.185, 866.385, 866.585, 866.785)
* EU868: 
	* 	3 Default channels(868.1, 868.3, 868.5)
	* 	5 Extra Channels (867.1, 867.3, 867.5, 867.7, 867.9)
* Transmission Power
	* RFO_HF:  14dBm output power
		* PABOOST: 0 [PA Disabled]
	* PA_BOOST: 20dBm output power
		* PABOOST: 1 [PA Enabled]
	* TX Power: 0
* PDS : Enabled
* HSM : Implemented

## Changelog

### V2.0.0
* TFLXLoRa Secure Element is Implemented. 

### V1.0.1
* Flash functionality added

### V1.0.0
* ASCII Command set support added. Similar to RN2483.
