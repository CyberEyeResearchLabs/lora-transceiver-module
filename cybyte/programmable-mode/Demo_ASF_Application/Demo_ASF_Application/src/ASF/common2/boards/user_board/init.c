/**
 * \file
 *
 * \brief User board initialization file
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <compiler.h>
#include <board.h>
#include <conf_board.h>
#include <port.h>

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Configure LEDs as outputs, turn them off */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_0_PIN, &pin_conf);
	port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
	
	/* Configure LEDs as outputs, turn them off */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_1_PIN, &pin_conf);
	port_pin_set_output_level(LED_1_PIN, LED_1_INACTIVE);
	
	/* Configure PA27 --> CPS as active */
	port_pin_set_config(CPS_PIN, &pin_conf);
	port_pin_set_output_level(CPS_PIN, CPS_ACTIVE);
	
	/* Configure PA13 --> CSD as active */
	port_pin_set_config(CSD_PIN, &pin_conf);
	port_pin_set_output_level(CSD_PIN, CSD_ACTIVE);
	
	#ifdef RFSWITCH_ENABLE
	/* Configure RFSWITCH as output */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(RF_SWITCH_PIN, &pin_conf);
	port_pin_set_output_level(RF_SWITCH_PIN, RF_SWITCH_INACTIVE);
	#endif

	#ifdef TCXO_ENABLE
	/* Configure TXPO PWR as output */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(TCXO_PWR_PIN, &pin_conf);
	port_pin_set_output_level(TCXO_PWR_PIN, TCXO_PWR_INACTIVE);
	#endif
}
