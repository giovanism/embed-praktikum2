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

static char strbuf[128];

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	board_init();

	/* Insert application code here, after the board has been initialized. */

	// Init lcd
	gfx_mono_init();	
	adc_sensors_init();
	
	// Inisialisasi interrupt vector
	pmic_init();
	cpu_irq_enable();
	
	// Engine Start On
	ioport_set_pin_level(LED0_GPIO, 0);

	//set background lcd on
	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);

	// Set display - output lcd
	gfx_mono_draw_string("Giovan Isa Musthofa",0, 0, &sysfont);
	delay_ms(1000);
	gfx_mono_draw_string("We will use sensor",0, 8, &sysfont);
	delay_ms(1000);
	gfx_mono_draw_string("    Sensor Read    ",0, 0, &sysfont);
	gfx_mono_draw_string("                   ",0, 8, &sysfont);
	
	for(;;)
	{
		ntc_measure();											// Mengambil data dari pengukuran suhu oleh NTC temperature sensor
		while(!ntc_data_is_ready());							// Menunggu data sampai siap untuk ditampilkan		
		volatile int8_t temperature = ntc_get_temperature();	// Mengambil hasil olah data dalam Celcius

		snprintf(strbuf, sizeof(strbuf), "Tempr : %3d", temperature);
		gfx_mono_draw_string(strbuf, 0, 8, &sysfont);
		
		// Check if button pressed
		if (ioport_get_pin_level(GPIO_PUSH_BUTTON_0) == 0) {
			// TODO implementasi klakson
		}
		if (ioport_get_pin_level(GPIO_PUSH_BUTTON_1) == 0) {
			// TODO implementasi jendela mobil
		}
		if (ioport_get_pin_level(GPIO_PUSH_BUTTON_2) == 0) {
			// TODO implementasi lampu sein
		}
		
		snprintf(strbuf, sizeof(strbuf), "Status : Mati", );
		gfx_mono_draw_string(strbuf, 0, 8, &sysfont);
	}
}
