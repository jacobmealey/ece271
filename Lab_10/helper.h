#ifndef HELPER_H
#define HELPER_H
#include "stm32l476xx.h"

void enable_HSI(void);
void configure_pins(int BUTTON_PIN, int LED_PIN);
void turn_on_LED(int LED_PIN);
void turn_off_LED(int LED_PIN);
void toggle_LED(int LED_PIN);
void stinky_delay(int ticks);
void foux_pwm(int period, double duty_cycle, int pin);

#endif