/**
 * @file lights.h
 * @brief Control de iluminación por PWM según nivel de luz ambiente.
 *
 * Este módulo contiene funciones para la inicialización y control de iluminación
 * en la pecera usando modulación por ancho de pulso (PWM). El brillo se ajusta
 * automáticamente en función de la lectura de un sensor de luz (fotocelda) conectado
 * al canal ADC 1 del microcontrolador.
 *
 * Se emplea una media móvil para suavizar las mediciones y evitar fluctuaciones
 * bruscas en el control de brillo.
 *
 * @author
 * Duván Felipe Vélez Restrepo
 * @date 2025
 */

#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include <stdint.h>

/**
 * @brief Inicializa el PWM para un GPIO determinado (~10 kHz).
 *
 * Configura el pin indicado como salida PWM y ajusta los registros para lograr
 * una frecuencia aproximada de 10 kHz, ideal para control de brillo sin parpadeo.
 *
 * @param gpio Número del pin GPIO a configurar como salida PWM.
 * @return Valor de 'top' calculado para esa frecuencia, necesario para definir el duty cycle.
 */
uint16_t pwm_init_basic(uint8_t gpio);

/**
 * @brief Controla el duty cycle del PWM según lectura del sensor de luz.
 *
 * Lee el canal ADC 1, aplica un filtro de media móvil para suavizar la señal,
 * y ajusta el nivel de PWM del pin `gpio_h` en proporción inversa a la luz ambiente.
 *
 * @param gpio_h Pin GPIO asociado al PWM (control del brillo).
 * @param top Valor de 'top' del PWM, usado para escalar el duty cycle.
 * @return Valor suavizado de luz (opcionalmente en voltios).
 */
float lights_control(uint8_t gpio_h, uint16_t top);

/**
 * @brief Aplica un filtro de media móvil a la señal de luz.
 *
 * Se utiliza internamente para suavizar los valores de ADC antes de
 * ajustar el brillo, reduciendo así parpadeos o transiciones bruscas.
 *
 * @param nuevo_valor Nueva lectura cruda del sensor.
 * @return Valor filtrado promedio.
 */
float media_movil2(float nuevo_valor);

#endif // _LIGHTS_H_
