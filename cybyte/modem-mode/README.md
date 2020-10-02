# CyByte - Modem Mode Stack #

This readme contains the default configuration that is set in the modem mode stack for CyByte.

## Modem Mode Command Set Capabilities: ##

* LoRaWAN Class A and C Protocol Compliant, and supports different lora Parameters which can be configured
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
* Intelligent Low-Power mode with programmable wake-up(after a specified time) and consumes ~14.83 in standby mode.
* All configuration and control have done over UART using simple ASCII commands with a baud rate of 115200
* PDS to store the LoRaWAN Parameters and join configuration using mac save and restore even after power-on reset or sys reset.
* Configurable Secure Join(HSM): Currently Disabled/Not implemented

## Default Configuration of the stack ##

* Transmission Type: Unconfirmed
* Device Class: Class A
* Data Rate: DR5
* ADR: False
* Freq Plan: IND865
* IND865 :
	* 3 Default Channels (865.0625, 865.4025, 865.985)
	* 5 Extra Channels (865.2325, 866.185, 866.385, 866.585, 866.785)
* Transmission Power
	* RFO_HF: FEM which yields 27dBm Output power
	* TX Power: 0
	* MAX EIRP: 14
* PDS : Enabled
* HSM : Not implemented
	
	