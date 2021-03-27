/**
 * \file
 *
 * \brief Light Dependent Resistor
 *
 */
#include <asf.h>
#include <sio2host.h>

/* ADC Instance */
struct adc_module adc_instance;

/* No of Samples to Read & buffer to store*/
#define ADC_SAMPLES			1
uint16_t adc_result_buffer[ADC_SAMPLES];

volatile bool adc_read_done = false;

#define DELAY_TO_READ_ADC_PIN    1				// For every one sec read the ADC value

/* Callback to trigger when ADC Conversion Done */
void adc_complete_callback(struct adc_module *const module)
{
	adc_read_done = true;
}

/* Configuring the ADC */
void configure_adc(void)
{
	struct adc_config config_adc;
	adc_get_config_defaults(&config_adc);
	
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV8;			// Pre-scaler - 8
	config_adc.reference       = ADC_REFERENCE_INTVCC2;				// Internal Reference Voltage - 3.3V
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN6;			// PA06
	config_adc.resolution      = ADC_RESOLUTION_12BIT;				// 12 bit resolution

	adc_init(&adc_instance, ADC, &config_adc);						// Initializing the ADC
	adc_enable(&adc_instance);										// Enabling the ADC
}

/* Configuring the ADC Callback */
void configure_adc_callbacks(void)
{
	adc_register_callback(&adc_instance,adc_complete_callback, ADC_CALLBACK_READ_BUFFER);
	adc_enable_callback(&adc_instance, ADC_CALLBACK_READ_BUFFER);
}

int main (void)
{
	/* Initializing the System, Sio2host, Delay Modules */
	system_init();
	sio2host_init();
	delay_init();

	/* Initializing the ADC, Callbacks */
	configure_adc();
	configure_adc_callbacks();
		
	/* Interrupt driven ADC*/
	system_interrupt_enable_global();
	
	enum status_code status;
	
	while(1){
		
		/* Reading Value from ADC Pin */
		status = adc_read_buffer_job(&adc_instance, adc_result_buffer, ADC_SAMPLES);
		
		if(status == 0){												// If successful status will be 0.
			printf("LDR Value: %d\n", adc_result_buffer[0]);			
		}
		else{
			printf("ADC Line Busy, Error Code: %08x\n", status);
		}
		
		while (adc_read_done == false) {
			/* Wait for asynchronous ADC read to complete */
		}
		delay_s(DELAY_TO_READ_ADC_PIN);						// Every one sec read the ADC value
	}
}
