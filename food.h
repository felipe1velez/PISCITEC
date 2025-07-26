/**
 * @file food.h
 * @brief Control de dispensador de alimento mediante servo PWM.
 *
 * Este archivo define las constantes y prototipos relacionados con el manejo del
 * dispensador de comida automática para el sistema Pecera Pro. Se controla mediante
 * un servomotor utilizando modulación PWM para abrir o cerrar el compartimento.
 *
 * Incluye funciones para inicializar el PWM del servo, convertir ángulos a valores de duty cycle,
 * y ejecutar la acción de apertura o cierre.
 *
 * @author
 * Duván Felipe Vélez Restrepo
 * @date 2025
 */

#ifndef _FOOD_H_
#define _FOOD_H_

#include <stdint.h>

// === Estados de apertura del dispensador ===
#define FOOD_OPEN   0       ///< Indica que se debe abrir el dispensador
#define FOOD_CLOSE  1       ///< Indica que se debe cerrar el dispensador

// === Tiempos recomendados de operación ===
#define OPEN_MS     100     ///< Duración en milisegundos del estado abierto
#define CLOSE_MS    5000    ///< Duración en milisegundos del estado cerrado

/**
 * @brief Controla el movimiento del servomotor para abrir o cerrar el dispensador.
 *
 * @param gpio Pin GPIO conectado al servo.
 * @param estado Estado deseado: `FOOD_OPEN` o `FOOD_CLOSE`.
 * @param top Valor máximo del contador PWM (frecuencia base).
 */
void food_control(uint8_t gpio, uint8_t estado, float top);

/**
 * @brief Convierte un ángulo deseado en grados a un valor de duty cycle.
 *
 * @param angulo Ángulo en grados (ej. 0–180).
 * @param top Valor de top del PWM para escalar correctamente el duty.
 * @return Valor de duty correspondiente.
 */
float angle_to_duty(float angulo, float top);

/**
 * @brief Inicializa el PWM en el pin del servo y retorna el valor de top (~50 Hz).
 *
 * @param gpio Número de pin GPIO que controla el servo.
 * @return Valor de top usado para esta frecuencia de operación.
 */
uint16_t servo_pwm_init(uint8_t gpio);

#endif // _FOOD_H_
