/**
 * @file lights.c
 * @brief Este documento contiene las funciones necesarias para manejar la luz.
 */

// Standard libraries
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Include your own header files here
#include "lights.h"

// Lee el ADC y lo convierte a voltaje
uint16_t read_lights() 
{
    adc_select_input(1);  // Canal 1 = GPIO27
    return adc_read();    // 0–4095 (12 bits)
}

// Check ADC level and assing PWM
float lights_control(uint8_t gpio_h, uint16_t top)
{
    
    uint16_t level = media_movil2(read_lights());
    uint32_t duty = 0;
    
    if (level < 500)
        duty = top * 1.0;  // 100%
    else if (level < 600)
        duty = top * 0.8;  // 80%
    else if (level < 800)
        duty = top * 0.5;  // 60%
    else if (level < 1100)
        duty = top * 0.3;  // 50%
    else if (level < 1600)
        duty = top * 0.1;  // 30%
    else
        duty = 0;          // luz muy alta → apagado
    
    pwm_set_gpio_level(gpio_h, duty);
    return level;
}

// Media móvil para suavizar lecturas
float media_movil2(float nuevo_valor) 
{
    static float buffer[10] = {0};
    static int indice = 0;
    static int contador = 0;

    buffer[indice] = nuevo_valor;
    indice = (indice + 1) % 10;

    if(contador < 10){ contador++; }

    float suma = 0;
    for(int i = 0; i < contador; i++)
    { suma += buffer[i]; }

    return suma / contador;
}

// pwm configuration 10kHz
uint16_t pwm_init_basic(uint8_t gpio) {
    // Configura el pin GPIO como salida PWM
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);

    // Calcula el valor de wrap (top) en función de la frecuencia PWM
    uint64_t clockspeed = clock_get_hz(clk_sys);
    uint16_t top = clockspeed / 10000;

    // Configura el PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, top);

    // Inicializa y habilita el PWM
    pwm_init(slice_num, &config, true);
    pwm_set_enabled(slice_num, true);

    return top;  // Retorna el valor de top calculado
}