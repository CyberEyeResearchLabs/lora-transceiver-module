/**
* \file  main.c
*
* \brief LORAWAN Demo Application main file
*		
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
 
/****************************** INCLUDES **************************************/
#include "system_low_power.h"
#include "radio_driver_hal.h"
#include "lorawan.h"
#include "sys.h"
#include "system_init.h"
#include "system_assert.h"
#include "aes_engine.h"
#include "enddevice_demo.h"
#include "sio2host.h"
#include "extint.h"
#include "conf_app.h"
#include "sw_timer.h"
#ifdef CONF_PMM_ENABLE
#include "pmm.h"
#include  "conf_pmm.h"
#include "sleep_timer.h"
#include "sleep.h"
#endif
#include "conf_sio2host.h"
#if (ENABLE_PDS == 1)
#include "pds_interface.h"
#endif
#include "sal.h"


#include "extint.h"
/************************** Macro definition ***********************************/
/* Button debounce time in ms */
#define APP_DEBOUNCE_TIME       50
/************************** Global variables ***********************************/
bool bandSelected = false;
uint8_t lTimerId = 0xFF;
uint8_t TxTimerId = 0xFF;
#ifdef CONF_PMM_ENABLE
bool deviceResetsForWakeup = false;
#endif
/************************** Extern variables ***********************************/

/************************** Function Prototypes ********************************/
static void driver_init(void);

#if (_DEBUG_ == 1)
static void assertHandler(SystemAssertLevel_t level, uint16_t code);
#endif /* #if (_DEBUG_ == 1) */

/*********************************************************************//**
 \brief      Uninitializes app resources before going to low power mode
*************************************************************************/
#ifdef CONF_PMM_ENABLE
static void app_resources_uninit(void);
#endif


// Required for button interface
static void ceye_configure_extint_channel(void);
static void ceye_extint_callback(void);
/****************************** FUNCTIONS **************************************/

// /* Configures External interrupt for Reed Switch on Both Edges{Rising as well as Falling} */
static void ceye_configure_extint_channel(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = BUTTON_0_PIN;
	config_extint_chan.gpio_pin_mux       = BUTTON_0_EIC_MUX;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_set_config(BUTTON_0_EIC_LINE, &config_extint_chan);

	/* Registering the Callback function to be triggered when the Interrupt Occurs */
	extint_register_callback(ceye_extint_callback,BUTTON_0_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
	
	/* Enable the Callback Function */
	extint_chan_enable_callback(BUTTON_0_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);

	/* Checking the Interrupt Channel for the Interrupt */
	while (extint_chan_is_detected(BUTTON_0_EIC_LINE)) {
		
		/* Interrupt Channel should be cleared, so that it can listen next interrupt */
		extint_chan_clear_detected(BUTTON_0_EIC_LINE);
	}
}

/* Callback Function - Make the flag 1 */
static void ceye_extint_callback(void)
{
	interrupt_flag = 1;					// This flag starts next transmission
	printf("EIR Trigged!!!\n");
}

static void print_reset_causes(void)
{
    enum system_reset_cause rcause = system_get_reset_cause();
    printf("Last reset cause: ");
    if(rcause & (1 << 6)) {
        printf("System Reset Request\r\n");
    }
    if(rcause & (1 << 5)) {
        printf("Watchdog Reset\r\n");
    }
    if(rcause & (1 << 4)) {
        printf("External Reset\r\n");
    }
    if(rcause & (1 << 2)) {
        printf("Brown Out 33 Detector Reset\r\n");
    }
    if(rcause & (1 << 1)) {
        printf("Brown Out 12 Detector Reset\r\n");
    }
    if(rcause & (1 << 0)) {
        printf("Power-On Reset\r\n");
    }
}

#ifdef CONF_PMM_ENABLE
static void appWakeup(uint32_t sleptDuration)
{
    HAL_Radio_resources_init();
    sio2host_init();
	
	/* FEM in Active mode (PA13 --> CSD) */
	port_pin_set_output_level(CSD_PIN, CSD_ACTIVE);
	
    printf("\r\nsleep_ok %ld ms\r\n", sleptDuration);

}
#endif

#if (_DEBUG_ == 1)
static void assertHandler(SystemAssertLevel_t level, uint16_t code)
{
    printf("\r\n%04x\r\n", code);
    (void)level;
}
#endif /* #if (_DEBUG_ == 1) */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the LORAWAN Demo Application of EU Band
 */
int main(void)
{
	
    /* System Initialization */
    system_init();
    /* Initialize the delay driver */
    delay_init();
    /* Initialize the board target resources */
    board_init();

    INTERRUPT_GlobalInterruptEnable();
	/* Initialize the Serial Interface */
	sio2host_init();

	//When working with switch or button
	ceye_configure_extint_channel();

	interrupt_flag = 1;
	
#ifndef CRYPTO_DEV_ENABLED
 	/* Read DEV EUI from EDBG */
    dev_eui_read();
#endif
    /* Initialize Hardware and Software Modules */
	driver_init();
   
    delay_ms(5);
    print_reset_causes();
#if (_DEBUG_ == 1)
    SYSTEM_AssertSubscribe(assertHandler);
#endif
    /* Initialize demo application */
    Stack_Init();

    SwTimerCreate(&lTimerId);
	SwTimerCreate(&TxTimerId);

    mote_demo_init();

    while (1)
    {
        SYSTEM_RunTasks();
#ifdef CONF_PMM_ENABLE

            if(bandSelected == true)
            {
                PMM_SleepReq_t sleepReq;
                /* Put the application to sleep */
                sleepReq.sleepTimeMs = DEMO_CONF_DEFAULT_APP_SLEEP_TIME_MS;
                sleepReq.pmmWakeupCallback = appWakeup;
                sleepReq.sleep_mode = CONF_PMM_SLEEPMODE_WHEN_IDLE;
                if (CONF_PMM_SLEEPMODE_WHEN_IDLE == SLEEP_MODE_STANDBY)
                {
                    deviceResetsForWakeup = false;
                }
                if (true == LORAWAN_ReadyToSleep(deviceResetsForWakeup))
                {
                    app_resources_uninit();
                    if (PMM_SLEEP_REQ_DENIED == PMM_Sleep(&sleepReq))
                    {
                        HAL_Radio_resources_init();
                        sio2host_init();
						
						/* FEM in Active mode (PA13 --> CSD) */
						port_pin_set_output_level(CSD_PIN, CSD_ACTIVE);
						
                        /*printf("\r\nsleep_not_ok\r\n");*/
                    }
                }
            }
#endif
    }
}

/* Initializes all the hardware and software modules used for Stack operation */
static void driver_init(void)
{
	SalStatus_t sal_status = SAL_SUCCESS;
    /* Initialize the Radio Hardware */
    HAL_RadioInit();
    /* Initialize the Software Timer Module */
    SystemTimerInit();
#ifdef CONF_PMM_ENABLE
    /* Initialize the Sleep Timer Module */
    SleepTimerInit();
#endif
#if (ENABLE_PDS == 1)
    /* PDS Module Init */
    PDS_Init();
#endif
	/* Initializes the Security modules */
	sal_status = SAL_Init();
	
	if (SAL_SUCCESS != sal_status)
	{
		printf("Initialization of Security module is failed\r\n");
		/* Stop Further execution */
		while (1) {
		}
	}
}

#ifdef CONF_PMM_ENABLE
static void app_resources_uninit(void)
{
    /* Disable USART TX and RX Pins */
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);
    pin_conf.powersave  = true;
    port_pin_set_config(HOST_SERCOM_PAD0_PIN, &pin_conf);
    port_pin_set_config(HOST_SERCOM_PAD1_PIN, &pin_conf);
    /* Disable UART module */
    sio2host_deinit();
    /* Disable Transceiver SPI Module */
    HAL_RadioDeInit();
	
	/* FEM in Shutdown mode (PA13 --> CSD) */
	port_pin_set_output_level(CSD_PIN, CSD_INACTIVE);
}
#endif
/**
 End of File
 */
