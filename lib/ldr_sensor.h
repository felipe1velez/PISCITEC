/**
 * @file ldr_sensor.h
 * @brief Interfaz del sensor LDR para medición de luz ambiente.
 *
 * Este archivo define las funciones necesarias para inicializar un canal ADC 
 * para la lectura de un sensor de luz tipo LDR (Light Dependent Resistor).
 * Se utiliza en el proyecto Pecera Pro para ajustar dinámicamente la iluminación.
 *
 * @author 
 * Duván Felipe Vélez Restrepo
 * @date 2025
 */

#ifndef LDR_SENSOR_H
#define LDR_SENSOR_H

#include <stdint.h>

/**
 * @brief Inicializa el canal ADC correspondiente al LDR.
 * 
 * @param adc_channel Canal del ADC (0-3) asociado al pin GPIO conectado al LDR.
 */
void ldr_init(uint adc_channel);

/**
 * @brief Lee el valor actual del sensor LDR desde el ADC.
 * 
 * @return Valor de 12 bits del ADC (rango 0–4095), proporcional a la luminosidad.
 */
uint16_t ldr_read(void);

#endif // LDR_SENSOR_H
