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
#if (CERT_APP == 1)
#include "conf_certification.h"
#include "enddevice_cert.h"
#endif
#include "sal.h"
#ifdef CRYPTO_DEV_ENABLED
#include "cryptoauthlib.h"
#include "conf_sal.h"
#endif
#include "config_at25dfx.h"

/************************** Macro definition ***********************************/
/* Button debounce time in ms */
#define APP_DEBOUNCE_TIME       50
/************************** Global variables ***********************************/
bool button_pressed = false;
bool factory_reset = false;
bool bandSelected = false;
uint32_t longPress = 0;
uint8_t demoTimerId = 0xFF;
uint8_t lTimerId = 0xFF;
extern bool certAppEnabled;
#ifdef CONF_PMM_ENABLE
bool deviceResetsForWakeup = false;
#endif
/************************** Extern variables ***********************************/
//! [driver_instances]
struct spi_module at25dfx_spi;
struct at25dfx_chip_module at25dfx_chip;
//! [driver_instances]
void at25dfx_deep_sleep(void);

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

/****************************** FUNCTIONS **************************************/

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

#ifdef CRYPTO_DEV_ENABLED
static void print_ecc_info(void)
{
	ATCA_STATUS  status;
	uint8_t    sn[9];			// ECC608A serial number (9 Bytes)
	uint8_t    info[2];
	uint8_t    tkm_info[10];
	int      slot = 10;
	int      offset = 70;
	uint8_t appEUI[8];
	uint8_t devEUIascii[16];
	uint8_t devEUIdecoded[8];	// hex.
	size_t bin_size = sizeof(devEUIdecoded);

	// read the serial number
	status = atcab_read_serial_number(sn);
	printf("\r\n--------------------------------\r\n");

	// read the SE_INFO
	status = atcab_read_bytes_zone(ATCA_ZONE_DATA, slot, offset, info, sizeof(info));
	
	// Read the CustomDevEUI
	status = atcab_read_bytes_zone(ATCA_ZONE_DATA, DEV_EUI_SLOT, 0, devEUIascii, 16);
	atcab_hex2bin((char*)devEUIascii, strlen((char*)devEUIascii), devEUIdecoded, &bin_size);

	// Print DevEUI
	printf("DEV EUI:  ");
	#if (SERIAL_NUM_AS_DEV_EUI == 1)
	print_array(sn, sizeof(sn)-1);
	#else
	print_array(devEUIdecoded, sizeof(devEUIdecoded));
	#endif
	
	// Read and Print the AppEUI
	status = atcab_read_bytes_zone(ATCA_ZONE_DATA, APP_EUI_SLOT, 0, appEUI, 8);
	printf("APP EUI:  ");
	print_array(appEUI, sizeof(appEUI));
	
	//Print Serial Number
	printf("SERIAL NUMBER:  ");
	print_array(sn, sizeof(sn));
	
	// assemble full TKM_INFO
	memcpy(tkm_info, info, 2);
	memcpy(&tkm_info[2], sn, 8);
	// tkm_info[] now contains the assembled tkm_info data
	printf("TKM INFO: ");
	print_array(tkm_info, sizeof(tkm_info));
	printf("--------------------------------\r\n");
}
#endif
/************************* START: FLASH MEMORY ***********************/
/**
 * \brief Initializing the Flash Memory
 */

//! [init_function]
static void at25dfx_init(void)
{
//! [config_instances]
	struct at25dfx_chip_config at25dfx_chip_config;
	struct spi_config at25dfx_spi_config;	
//! [config_instances]

//! [spi_setup]
	at25dfx_spi_get_config_defaults(&at25dfx_spi_config);
	at25dfx_spi_config.mode_specific.master.baudrate = AT25DFX_CLOCK_SPEED;
	at25dfx_spi_config.mux_setting = AT25DFX_SPI_PINMUX_SETTING;
	at25dfx_spi_config.pinmux_pad0 = AT25DFX_SPI_PINMUX_PAD0;
	at25dfx_spi_config.pinmux_pad1 = AT25DFX_SPI_PINMUX_PAD1;
	at25dfx_spi_config.pinmux_pad2 = AT25DFX_SPI_PINMUX_PAD2;
	at25dfx_spi_config.pinmux_pad3 = AT25DFX_SPI_PINMUX_PAD3;

	spi_init(&at25dfx_spi, AT25DFX_SPI, &at25dfx_spi_config);
	spi_enable(&at25dfx_spi);
		
//! [spi_setup]

//! [chip_setup]
	at25dfx_chip_config.type = AT25DFX_MEM_TYPE;
	at25dfx_chip_config.cs_pin = AT25DFX_CS;

	at25dfx_chip_init(&at25dfx_chip, &at25dfx_spi, &at25dfx_chip_config);
//! [chip_setup]

}
//! [init_function]

/**
 * \brief Flash Memory to deep sleep
 */
void at25dfx_deep_sleep(void)
{
	enum status_code status;
	
	status = at25dfx_chip_wake(&at25dfx_chip);
// 	printf("Waking up Serial Flash              : %s\r\n", status?"NOT_OK":"OK");
	
	delay_ms(500);
	
	status = at25dfx_chip_sleep(&at25dfx_chip);
// 	printf("Keeping Flash in Deep Sleep Mode    : %s\r\n", status?"NOT_OK":"OK");
}

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

    SwTimerCreate(&demoTimerId);
    SwTimerCreate(&lTimerId);
#ifdef CRYPTO_DEV_ENABLED
	print_ecc_info();
#endif

    mote_demo_init();

    while (1)
    {
		serial_data_handler();
        SYSTEM_RunTasks();
#ifdef CONF_PMM_ENABLE
        if (false == certAppEnabled)
        {
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

	/* Initialize Flash Module */
	at25dfx_init();

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
	at25dfx_deep_sleep();
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
