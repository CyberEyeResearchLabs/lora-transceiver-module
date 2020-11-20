/**
* \file
*
* \brief AT25DFx SerialFlash driver quick start
*
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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

#include <asf.h>
#include <sio2host.h>
#include "config_at25dfx.h"


//! [buffers]
#define AT25DFX_BUFFER_SIZE  (10)

static uint8_t read_buffer[AT25DFX_BUFFER_SIZE];
static uint8_t write_buffer[AT25DFX_BUFFER_SIZE] = {1, 8, 2, 3, 6, 1, 6, 8, 8, 9};
//! [buffers]

//! [driver_instances]
struct spi_module at25dfx_spi;
struct at25dfx_chip_module at25dfx_chip;
//! [driver_instances]

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
static void at25dfx_deep_sleep(void)
{
	enum status_code status;
	
	status = at25dfx_chip_wake(&at25dfx_chip);
	printf("Waking up Serial Flash              : %s\r\n", status?"NOT_OK":"OK");
	
	delay_ms(1000);
	
	status = at25dfx_chip_sleep(&at25dfx_chip);
	printf("Keeping Flash in Deep Sleep Mode    : %s\r\n", status?"NOT_OK":"OK");
}

/**
 * \brief Flash Memory to deep sleep
 */
static void at25dfx_ultra_deep_sleep(void)
{
	enum status_code status;
	status = at25dfx_chip_wake(&at25dfx_chip);
	printf("Waking up Serial Flash                : %s\r\n", status?"NOT_OK":"OK");
	
	delay_ms(1000);
	
	status = at25dfx_chip_ultra_sleep(&at25dfx_chip);
	printf("Keeping Flash in Ultra Deep Sleep Mode: %s\r\n", status?"NOT_OK":"OK");

}


int main(void)
{
//! [init_calls]
	system_init();
	
	sio2host_init();
	
	printf("\n///////////// Application: Flash Memory - Basic Read, Write, Sleep /////////////\n\n");
	enum status_code status, read_status, write_status;
	at25dfx_init();
//! [init_calls]

//! [use_code]
//! [wake_chip]	
	status = at25dfx_chip_wake(&at25dfx_chip);
	printf("Waking up Serial Flash             : %s\r\n", status?"NOT_OK":"OK");
	printf("chip_wake() ret=0x%08X\r\n\n", status);
	
//! [wake_chip]
	
// ! [check_presence]
// 	status = at25dfx_chip_check_presence(&at25dfx_chip);
// 	printf("Presence of Serial Flash          : %s\r\n", status?"NOT_PRESENT":"PRESENT");
// 	if (status != STATUS_OK) {
// 		// Handle missing or non-responsive device
// 		printf("\nSerial Flash is not present\n");
// 	}
// ! [check_presence]
	
//! [read_buffer]
	read_status = at25dfx_chip_read_buffer(&at25dfx_chip, 0x0000, read_buffer, AT25DFX_BUFFER_SIZE);
	printf("Read from 0x0000 Memory            : %s\r\n", read_status?"NOT_OK":"OK");
	printf("read_buffer() ret=0x%08X\r\n", read_status);
	printf("\n-------------------------");
	printf("\nBuffer Data at 0x0000 ::>>  ");
	for(int i=0; i <AT25DFX_BUFFER_SIZE;i++){
		printf("%d", read_buffer[i]);		
	}
	printf("\n-------------------------\n");
//! [read_buffer]

//! [unprotect_sector]
	status = at25dfx_chip_set_global_sector_protect(&at25dfx_chip, false);
	printf("Unprotect sector		   : %s\r\n", status?"NOT_OK":"OK");
	printf("global_sector_unprotect() ret=0x%08X\r\n\n", status);
	
// 	status = at25dfx_chip_set_sector_protect(&at25dfx_chip, 0x10000, false);
// 	printf("Unprotect sector address 0x10000   : %s\r\n", status?"NOT_OK":"OK");
// 	printf("at25dfx_chip_set_sector_protect() failed with ret=0x%08X\r\n\n", status);
//! [unprotect_sector]

//! [erase_block]
	status = at25dfx_chip_erase_block(&at25dfx_chip, 0x10000, AT25DFX_BLOCK_SIZE_4KB);
	printf("Erase block address 0x10000        : %s\r\n", status?"NOT_OK":"OK");
	printf("erase_block() ret=0x%08X\r\n\n", status);
//! [erase_block]


//! [read_buffer]
	uint8_t buffer[AT25DFX_BUFFER_SIZE];
	read_status = at25dfx_chip_read_buffer(&at25dfx_chip, 0x10000, buffer, AT25DFX_BUFFER_SIZE);
	printf("Read from 0x10000 Memory           : %s\r\n", read_status?"NOT_OK":"OK");
	printf("read_buffer() ret=0x%08X\r\n", read_status);
	printf("\n-------------------------");
	printf("\nBuffer Data at 0x10000 ::>>  ");
	for(int i=0; i <AT25DFX_BUFFER_SIZE;i++){
		printf("%d", buffer[i]);
	}
	printf("\n-------------------------\n");
//! [read_buffer]
	
		
//! [write_buffer]
	write_status = at25dfx_chip_write_buffer(&at25dfx_chip, 0x10000, write_buffer, AT25DFX_BUFFER_SIZE);
	printf("Write to 0x10000 Memory            : %s\r\n", write_status?"NOT_OK":"OK");
	printf("write_buffer() ret=0x%08X\r\n\n", write_status);
//! [write_buffer]

//! [get data from 0x10000]
	read_status = at25dfx_chip_read_buffer(&at25dfx_chip, 0x10000, buffer, AT25DFX_BUFFER_SIZE);
	printf("Read from 0x10000 Memory           : %s\r\n", read_status?"NOT_OK":"OK");
	printf("read_buffer() ret=0x%08X\r\n", read_status);
	printf("\n-------------------------");
	printf("\nBuffer Data at 0x10000 ::>>  ");
	for(int i=0; i <AT25DFX_BUFFER_SIZE;i++){
		printf("%d", buffer[i]);
	}
	printf("\n-------------------------\n");
//! [get data from 0x10000]

// //! [global_protect]
	status = at25dfx_chip_set_global_sector_protect(&at25dfx_chip, true);
	printf("Protect sector			   : %s\r\n", status?"NOT_OK":"OK");
	printf("global_sector_protect() ret=0x%08X\r\n\n", status);
// //! [global_protect]

//! [sleep] 
	status = at25dfx_chip_sleep(&at25dfx_chip);
	printf("Keeping Flash in Deep Sleep Mode   : %s\r\n", status?"NOT_OK":"OK");
	printf("at25dfx_chip_sleep() ret=0x%08X\r\n\n", status);
//! [sleep]

// ! [use_code]
}
