/**
 * @file temperature.h
 * @brief Módulo para lectura y control de temperatura del agua en sistemas embebidos.
 *
 * Este archivo define las funciones y macros necesarias para medir la temperatura del agua
 * usando el sensor LM35 conectado al ADC de la Raspberry Pi Pico, y aplicar un control de 
 * calentador ON/OFF según umbrales definidos. También implementa una media móvil para 
 * suavizar las lecturas.
 *
 * ## Funcionalidades:
 * - Inicialización del ADC para el canal correspondiente al LM35.
 * - Conversión de voltaje ADC a temperatura en grados Celsius.
 * - Control automático del calentador por histéresis (ON/OFF).
 * - Filtrado de lectura de temperatura con media móvil.
 *
 * @author 
 * Duván Felipe Vélez Restrepo
 * @date 2025
 */

#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

/**
 * @brief Umbral superior para activar el apagado del calentador.
 */
#define HOT_TEMPERATURE 26.0f

/**
 * @brief Umbral inferior para encender el calentador.
 */
#define COLD_TEMPERATURE 25.0f

/**
 * @brief Tamaño de la ventana para aplicar la media móvil sobre la temperatura.
 */
#define TEMP_WINDOW_SIZE 10

/**
 * @brief Inicializa el ADC para el canal especificado.
 * 
 * @param input_channel Canal del ADC donde está conectado el sensor LM35.
 */
void init_adc(uint8_t input_channel);

/**
 * @brief Lee y convierte la temperatura actual desde el ADC.
 * 
 * @return Temperatura en grados Celsius.
 */
float read_temperature();

/**
 * @brief Controla el estado de un calentador conectado a un GPIO.
 * 
 * Enciende o apaga el calentador según la temperatura medida con histéresis.
 * 
 * @param gpio_h GPIO de control del calentador.
 * @return Temperatura actual en °C (filtrada).
 */
float temperature_control(uint8_t gpio_h);

/**
 * @brief Aplica una media móvil a la temperatura leída.
 * 
 * @param nuevo_valor Nueva lectura de temperatura.
 * @return Temperatura suavizada.
 */
float media_movil(float nuevo_valor);

#endif // _TEMPERATURE_H_
