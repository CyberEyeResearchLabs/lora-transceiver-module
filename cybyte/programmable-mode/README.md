# README #

This folder contains basic transmission codes for IND865 and its LoRa parameters.

##### Parameters used in the Application #####

* Join Mode: OTAA
* Device Class: Class A
* Transmission Type: Unconfirmed
* Data Rate: DR5
* ADR: False
* Transmission Interval between uplinks: 2s [If TX_DELAY Macro is defined]
* Sleep Duration: 10s [If DEVICE_SLEEP Macro is defined]
* Continous transmissions
* Freq Plan: IND865
* IND865 : 
	* 3 Default Channels (865.0625, 865.4025, 865.985)
	* 5 Extra Channels (865.2325, 866.185, 866.385, 866.585, 866.785)
* Transmission Power
	* RFO_HF: FEM which yields 27dBm Output power
	* TX Power: 0
	* MAX EIRP: 14
* PDS : Enabled
* HSM: No

Note: Default Configuration is different from one project to another.