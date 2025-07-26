/**
 * @file temperature.h
 * @brief This is a brief description of the food H file.
 *
 * Detailed description of the temperature C file.
 */
 
// Avoid duplication in code
#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

// Write your definitions and other macros here
#define HOT_TEMPERATURE 26.0f
#define COLD_TEMPERATURE 25.0f
#define TEMP_WINDOW_SIZE 10  // Tamaño de la ventana para la media móvil

void init_adc(uint8_t input_channel);
float read_temperature();
float temperature_control(uint8_t gpio_h);
float media_movil(float nuevo_valor);

#endif