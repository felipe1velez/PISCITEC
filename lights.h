/**
 * @file lights.h
 * @brief Control de iluminación por PWM según nivel de luz ambiente.
 *
 * Contiene funciones para inicializar el PWM y controlar su duty cycle de acuerdo
 * con la lectura de una fotocelda conectada al canal ADC 1.
 */

#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include <stdint.h>

/**
 * @brief Inicializa el PWM para un GPIO determinado (~10 kHz).
 * @param gpio Número del pin GPIO a configurar como salida PWM.
 * @return Valor de 'top' calculado para esa frecuencia.
 */
uint16_t pwm_init_basic(uint8_t gpio);
/**
 * @brief Lee el ADC 1 y calcula el duty cycle según el nivel de luz.
 * @param gpio_h Pin GPIO asociado al PWM.
 * @param top Valor de 'top' del PWM, usado para escalar el duty.
 */
float lights_control(uint8_t gpio_h, uint16_t top);

float media_movil2(float nuevo_valor);

#endif // _LIGHTS_H_