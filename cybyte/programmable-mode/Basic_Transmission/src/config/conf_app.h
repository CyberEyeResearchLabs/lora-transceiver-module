/**
* \file  conf_app.h
*
* \brief LORAWAN Demo Application include file
*		
*
* Copyright (c) 2019 Microchip Technology Inc. and its subsidiaries. 
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products. 
* It is your responsibility to comply with third party license terms applicable 
* to your use of third party software (including open source software) that 
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

 

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/****************************** INCLUDES **************************************/

/****************************** MACROS **************************************/
/* Number of software timers */
#define TOTAL_NUMBER_OF_TIMERS            (25u)


/*Define the Sub band of Channels to be enabled by default for the application*/
#define SUBBAND 1
#if ((SUBBAND < 1 ) || (SUBBAND > 8 ) )
#error " Invalid Value of Subband"
#endif

/* Activation method constants */
#define OVER_THE_AIR_ACTIVATION           LORAWAN_OTAA
#define ACTIVATION_BY_PERSONALIZATION     LORAWAN_ABP

/* Message Type constants */
#define UNCONFIRMED                       LORAWAN_UNCNF
#define CONFIRMED                         LORAWAN_CNF

/* Enable one of the activation methods */
#define DEMO_APP_ACTIVATION_TYPE          OVER_THE_AIR_ACTIVATION
// #define DEMO_APP_ACTIVATION_TYPE       ACTIVATION_BY_PERSONALIZATION

/* Select the Type of Transmission - Confirmed(CNF) / Unconfirmed(UNCNF) */
#define DEMO_APP_TRANSMISSION_TYPE        UNCONFIRMED
// #define DEMO_APP_TRANSMISSION_TYPE     CONFIRMED

/* FPORT Value (1-255) */
#define DEMO_APP_FPORT                    1

/* Device Class - Class of the device (CLASS_A/CLASS_C) */
#define DEMO_APP_ENDDEVICE_CLASS          CLASS_A
// #define DEMO_APP_ENDDEVICE_CLASS			CLASS_C

/* Uncomment If you want to test continuous downlinks and after first 5 uplinks */
// #define	 SINGLE_UPLINK

/* ADR - State of the ADR (Enable(True)/Disable(False)) */
#define ADR_STATE							false

/* Uncomment when delay between two transmissions is required
   Comment when delay between two transmission is not required */
#define TX_DELAY

/* Configure the Delay Interval between two transmissions in milliseconds */
#define TX_INTERVAL								2000	

/* Uncomment when Device should sleep between two transmissions is required
   Comment when Device should not sleep between two transmission */
// #define DEVICE_SLEEP

/* This macro defines the application's default sleep duration in milliseconds */
#define DEMO_CONF_DEFAULT_APP_SLEEP_TIME_MS     10000

/* ABP Join Parameters */
#define DEMO_DEVICE_ADDRESS                     0x270000DC
#define DEMO_APPLICATION_SESSION_KEY            {0x2D, 0x7E, 0x96, 0x11, 0x68, 0xAB, 0x3C, 0x9D, 0x0B, 0xC0, 0x0A, 0x4B, 0x1D, 0xAF, 0x70, 0x08}
#define DEMO_NETWORK_SESSION_KEY                {0x69, 0xC4, 0x50, 0x4B, 0xE9, 0x41, 0xDF, 0xA9, 0x85, 0x11, 0xF9, 0x66, 0x9C, 0x3B, 0x82, 0xE1}

/* OTAA Join Parameters */
// #define DEMO_DEVICE_EUI                         {0x12, 0x31, 0x23, 0x12, 0x31, 0x23, 0x12, 0x31 }
// #define DEMO_APPLICATION_EUI                    {0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23 }
// #define DEMO_APPLICATION_KEY                    { 0x52, 0x0E, 0x21, 0xF3, 0xBE, 0x7B, 0x64, 0xA4, 0x97, 0x71, 0x68, 0xF4, 0xA5, 0x66, 0xD2, 0x5C }


#define DEMO_DEVICE_EUI                         { 0x12, 0x5A, 0xD5, 0x4F, 0xA4, 0x65, 0xD4, 0x65}
#define DEMO_APPLICATION_EUI                    { 0x55, 0xFF, 0xA2, 0xB3, 0xF4, 0xC6, 0xD8, 0x55 }
#define DEMO_APPLICATION_KEY                    { 0xE7, 0x71, 0x91, 0xB9, 0xC8, 0x10, 0xF9, 0xF8, 0x86, 0xEF, 0x14, 0x49, 0x6A, 0xE5, 0x15, 0x45 }
	
/* Multicast Parameters */
#define DEMO_APP_MCAST_GROUPID                  0
#define DEMO_APP_MCAST_ENABLE                   false
#define DEMO_APP_MCAST_GROUP_ADDRESS            0x0037CC56
#define DEMO_APP_MCAST_APP_SESSION_KEY          {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6}
#define DEMO_APP_MCAST_NWK_SESSION_KEY          {0x3C, 0x8F, 0x26, 0x27, 0x39, 0xBF, 0xE3, 0xB7, 0xBC, 0x08, 0x26, 0x99, 0x1A, 0xD0, 0x50, 0x4D}

#endif /* APP_CONFIG_H_ */

