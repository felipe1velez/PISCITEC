/**
 * @file food.c
 * @brief Control del dispensador de alimento usando un servomotor mediante PWM.
 *
 * Este módulo proporciona las funciones necesarias para manejar el dispensador
 * de comida del sistema Pecera Pro, controlado por un servomotor. Las funciones
 * permiten abrir o cerrar el compartimento de alimento, calcular el duty cycle
 * necesario para cada posición del servo y configurar adecuadamente el PWM.
 *
 * El ángulo del servo se convierte en un pulso PWM dentro de un ciclo de 20 ms,
 * oscilando típicamente entre 1 ms (0°) y 2 ms (180°). Se incluyen valores fijos
 * de corrección (`fix`) y compensación (`ang`) para calibrar la posición física
 * del mecanismo según el diseño mecánico específico del dispensador.
 *
 * @author
 * Duván Felipe Vélez Restrepo
 * @date 2025
 */

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"

#include "food.h"

float top_g;         ///< Valor global de top PWM actual
uint8_t servo_g;     ///< GPIO global asociado al servo

/**
 * @brief Controla el movimiento del servo para abrir o cerrar el compartimento de comida.
 *
 * Define internamente el ángulo de apertura y cierre, y convierte estos valores
 * a ciclos de trabajo para el PWM. El comportamiento se ajusta mediante un valor
 * de corrección (`fix`) y una compensación angular (`ang`) para ajustar
 * físicamente el mecanismo.
 *
 * @param servo_gpio GPIO conectado al servomotor.
 * @param food_case Estado deseado del dispensador (`FOOD_OPEN` o `FOOD_CLOSE`).
 * @param top Valor de top del PWM (frecuencia base).
 */
void food_control(uint8_t servo_gpio, uint8_t food_case, float top)
{
    top_g = top;
    servo_g = servo_gpio;

    float fix = 35;     // Compensación de hardware
    int ang = 20;       // Margen de apertura deseado

    float duty_cycle;
    switch(food_case)
    {
        case FOOD_OPEN:
            duty_cycle = angle_to_duty(140 - ang, fix);
            break;
        case FOOD_CLOSE:
            duty_cycle = angle_to_duty(140, fix);
            break;
        default:
            duty_cycle = angle_to_duty(140, fix);
            break;
    }

    pwm_set_gpio_level(servo_gpio, top * duty_cycle);
}

/**
 * @brief Convierte un ángulo a duty cycle normalizado para un servo.
 *
 * La conversión toma como base un pulso mínimo de 1 ms (0°) y máximo de 2 ms (180°),
 * dentro de un ciclo de 20 ms. El valor `fix` representa una corrección de desplazamiento
 * para adaptarse al montaje físico del servo.
 *
 * @param angle Ángulo deseado en grados (0° a 180°).
 * @param fix Corrección fija para ajustar la mecánica real del servo.
 * @return Duty cycle como valor decimal entre 0.0 y 1.0.
 */
float angle_to_duty(float angle, float fix)
{
    float pulse_width_us = 1000.f + ((angle - fix) * 1000.f / 93.f);
    float duty_cycle = pulse_width_us / 20000.f;
    return duty_cycle;
}

/**
 * @brief Inicializa el PWM para el pin GPIO del servo (~50 Hz).
 *
 * Calcula automáticamente el divisor de reloj y el valor de wrap para lograr
 * una frecuencia de PWM compatible con servomotores estándar (~50 Hz). Configura
 * el PWM en el pin especificado y lo deja listo para uso inmediato.
 *
 * @param servo_gpio Número del pin GPIO usado por el servomotor.
 * @return Valor del contador de top calculado para esa frecuencia.
 */
uint16_t servo_pwm_init(uint8_t servo_gpio)
{
    gpio_set_function(servo_gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(servo_gpio);

    float clockDiv = 64;
    float wrap = 39062;
    pwm_config config = pwm_get_default_config();

    uint64_t clockspeed = clock_get_hz(clk_sys);

    while (clockspeed / clockDiv / 50 > 65535 && clockDiv < 256)
        clockDiv += 64;

    wrap = clockspeed / clockDiv / 50;

    pwm_config_set_clkdiv(&config, clockDiv);
    pwm_config_set_wrap(&config, wrap);

    pwm_init(slice_num, &config, true);

    return wrap;
}
