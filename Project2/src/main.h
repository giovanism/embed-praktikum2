/*
 * main.h
 *
 * Created: 10/16/2019 7:59:28 PM
 *  Author: Giovan
 */ 


#ifndef MAIN_H_
#define MAIN_H_

int main(void);
void PWM_Init(void);
void potensiometer_init(void);
void setup_timer0(void);
int16_t potensiometer_get_value(void);
static void hold_window(int compare);
void handler_blink(void);
void blink(void);

#endif /* MAIN_H_ */