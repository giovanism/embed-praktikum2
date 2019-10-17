/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include "adc_sensors/adc_sensors.h"
#include "main.h"

#define WINDOW_OPEN 150
#define WINDOW_SHUT 1

static const char ON_STR[] = "On";
static const char OFF_STR[] = "Off";

static char strbuf[128];
static char* turn_sign = OFF_STR;

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

	board_init();
	PWM_Init();

	/* Insert application code here, after the board has been initialized. */

	// Init lcd
	gfx_mono_init();
	adc_sensors_init();
	potensiometer_init();

	// Inisialisasi interrupt vector
	pmic_init();
	cpu_irq_enable();

	// Engine Start On
	ioport_set_pin_level(LED0_GPIO, 0);

	//set background lcd on
	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);

	// Set display - output lcd
	gfx_mono_draw_string("   Sensor Read    ",0, 0, &sysfont);
	delay_ms(1000);
	gfx_mono_draw_string("We will use sensor",0, 8, &sysfont);
	delay_ms(1000);
	gfx_mono_draw_string("Giovan Isa Musthofa",0, 0, &sysfont);
	gfx_mono_draw_string("                   ",0, 8, &sysfont);
	
	// Set J4 Pin 0 to output
	ioport_set_pin_dir(J4_PIN0, IOPORT_DIR_OUTPUT);
	
	// Set up timer interrupt
	setup_timer0();

	int window_pos;
	int	speed;
	int sound;

	for(;;)
	{
		ntc_measure();											// Mengambil data dari pengukuran suhu oleh NTC temperature sensor
		while(!ntc_data_is_ready());							// Menunggu data sampai siap untuk ditampilkan
		volatile int8_t temperature = ntc_get_temperature();	// Mengambil hasil olah data dalam Celcius

		snprintf(strbuf, sizeof(strbuf), "Tempr : %3d", temperature);
		gfx_mono_draw_string(strbuf, 0, 8, &sysfont);

		window_pos = WINDOW_SHUT;
		speed = potensiometer_get_value();
		sound = 0;

		// Check if button pressed
		if (ioport_get_pin_level(GPIO_PUSH_BUTTON_0) == 0) {
			// Implementasi klakson
			sound = 1;
		}
		if (ioport_get_pin_level(GPIO_PUSH_BUTTON_1) == 0) {
			// Implementasi jendela mobil
			window_pos = WINDOW_OPEN;
		}
		if (ioport_get_pin_level(GPIO_PUSH_BUTTON_2) == 0) {
			// Switch on or off turn sign
			if (turn_sign == OFF_STR) {
				turn_sign = ON_STR;
			} else {
				turn_sign = OFF_STR;	
			}
		}

		// Move window to position
		hold_window(window_pos);
		// Set klaxon pin level
		ioport_set_pin_level(J4_PIN0, sound);
		//gpio_set_pin_high(J4_PIN0);
		
		snprintf(strbuf, sizeof(strbuf), "Status : %3s", turn_sign);
		gfx_mono_draw_string(strbuf, 0, 16, &sysfont);
		snprintf(strbuf, sizeof(strbuf), "Speed : %4d", speed);
		gfx_mono_draw_string(strbuf, 0, 24, &sysfont);
	}
}


void PWM_Init(void)
{
	// Set output
	PORTC.DIR |= PIN0_bm;

	// Set Register
	TCC0.CTRLA = (PIN2_bm) | (PIN0_bm);
	TCC0.CTRLB = (PIN4_bm) | (PIN2_bm) | (PIN1_bm);

	// Set Period
	TCC0.PER = 1000;

	// Set Compare Register value
	TCC0.CCA = 375;
}

void potensiometer_init(void) {
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;

	adc_read_configuration(&ADCA, &adc_conf);
	adcch_read_configuration(&ADCA, ADC_CH2, &adcch_conf);

	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,ADC_REF_VCC);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 125000UL);

	adcch_set_input(&adcch_conf, J2_PIN0, ADCCH_NEG_NONE, 1);

	adc_write_configuration(&ADCA, &adc_conf);
	adcch_write_configuration(&ADCA, ADC_CH2, &adcch_conf);
}

/**
 * \brief Read the potensiometer value from the ADC
 *
 * This will read the ADC value of the channel and pin connected to the
 * potensiometer on the A3BU-Xplained.
 *
 * \retval the raw ADC value from the current potensiometer
 */
int16_t potensiometer_get_value(void) {
	uint16_t result;
	adc_enable(&ADCA);
	adc_start_conversion(&ADCA, ADC_CH2);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH2);
	result = adc_get_result(&ADCA, ADC_CH2);
	return result;
}


static void hold_window(int compare){
	TCC0.CCA = compare;
}

void setup_timer0(void){
	
	tc_enable(&TCD0);
	tc_set_overflow_interrupt_callback(&TCD0, handler_blink);
	tc_set_wgm(&TCD0, TC_WG_NORMAL);
	tc_write_period(&TCD0, 4000);
	tc_set_overflow_interrupt_level(&TCD0, TC_INT_LVL_LO);
	tc_write_clock_source(&TCD0, TC_CLKSEL_DIV1024_gc);
	
	cpu_irq_enable();
}

void handler_blink(void) {
	if (turn_sign == ON_STR) {
		blink();
	} else {
		ioport_set_pin_level(LED1_GPIO, 1);
		ioport_set_pin_level(LED1_GPIO, 1);
	}
}

void blink(void) {
	gpio_toggle_pin(LED1);
	gpio_toggle_pin(LED2);
}
