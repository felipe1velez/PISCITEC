/**
 * @file food.c
 * @brief Este documento contiene las funciones necesarias para manejar el dispensador de comida.
 */

// Standard libraries
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"

// Include your own header files here
#include "food.h"

float top_g;
uint8_t servo_g;

void food_control(uint8_t servo_gpio, uint8_t food_case, float top)
{
    top_g = top;
    servo_g = servo_gpio;

    float fix = 35;
    int ang = 20;

    float duty_cycle;
    switch(food_case)
    { 
        case FOOD_OPEN: 
        duty_cycle = angle_to_duty(140-ang, fix);
        break;

        case FOOD_CLOSE: 
        duty_cycle = angle_to_duty(140, fix);//145
        break;

        default: 
        duty_cycle = angle_to_duty(140, fix);
        break;
    }
    pwm_set_gpio_level(servo_gpio, top * duty_cycle);
}

// Calcula el ciclo de trabajo para el ángulo deseado 
float angle_to_duty(float angle, float fix)
{ 
    float pulse_width_us = 1000.f + ((angle-fix) * 1000.f / 93.f); // 1000 us para 0 grados, 2000 us para 180 grados 
    float duty_cycle = (pulse_width_us / 20000.f) ; // Convertir a duty cycle de PWM 
    return duty_cycle;
}

//Inicializa el PWM para el gpio deseado
uint16_t servo_pwm_init(uint8_t servo_gpio)
{
    // Configura el pin GPIO para PWM 
    gpio_set_function(servo_gpio, GPIO_FUNC_PWM); 
    uint slice_num = pwm_gpio_to_slice_num(servo_gpio);  

    float clockDiv = 64;
    float wrap = 39062; 
    pwm_config config = pwm_get_default_config();
    
    uint64_t clockspeed = clock_get_hz(5);

    while (clockspeed/clockDiv/50 > 65535 && clockDiv < 256) clockDiv += 64; 
    wrap = clockspeed/clockDiv/50;

    pwm_config_set_clkdiv(&config, clockDiv);
    pwm_config_set_wrap(&config, wrap);

    pwm_init(slice_num, &config, true);

    return wrap;
}
