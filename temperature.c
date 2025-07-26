/**
 * @file temperature.c
 * @brief Implementación del módulo de control de temperatura para el sistema Piscitec.
 *
 * Este archivo contiene la lógica de lectura del sensor LM35 conectado al ADC,
 * conversión de la señal analógica a temperatura en grados Celsius, aplicación
 * de una media móvil para estabilizar la lectura, y control del GPIO que activa
 * o desactiva el calentador según los umbrales definidos.
 *
 * ## Funcionalidades:
 * - Lectura del ADC y conversión a temperatura en °C.
 * - Suavizado de la lectura mediante media móvil.
 * - Activación/desactivación del calentador con histéresis.
 * - Inicialización del canal ADC para el LM35.
 *
 * @author
 * Duván Felipe Vélez Restrepo
 * @date 2025
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "temperature.h"

/// Estado interno del calentador (true si está encendido)
bool heater_on = false;

/**
 * @brief Lee la señal del sensor LM35 y la convierte a temperatura (°C).
 *
 * Selecciona el canal ADC correspondiente al sensor, realiza la lectura cruda
 * de 12 bits y convierte el valor a temperatura en grados Celsius.
 *
 * @return Temperatura medida (sin filtrar), en grados Celsius.
 */
float read_temperature() 
{
    adc_select_input(0);
    uint16_t raw = adc_read();              // Lectura cruda (12 bits)
    return (raw * 3.3f / 4095) * 100;       // Conversión a °C
}

/**
 * @brief Controla el estado del calentador según la temperatura.
 *
 * Aplica histéresis entre `COLD_TEMPERATURE` y `HOT_TEMPERATURE`.
 * Utiliza una media móvil para tomar decisiones estables.
 *
 * @param gpio_h GPIO conectado al calentador (activo en alto).
 * @return Temperatura filtrada usada para el control.
 */
float temperature_control(uint8_t gpio_h)
{
    float temp = media_movil(read_temperature());

    if(temp > HOT_TEMPERATURE) {
        if(heater_on) {
            gpio_put(gpio_h, 0);
            heater_on = false;
        }
    } 
    else if(temp < COLD_TEMPERATURE) {
        if(!heater_on) {
            gpio_put(gpio_h, 1);
            heater_on = true;
        }
    }
    return temp;
}

/**
 * @brief Aplica una media móvil a una secuencia de temperaturas.
 *
 * Usa una ventana de tamaño `TEMP_WINDOW_SIZE` para suavizar la lectura.
 *
 * @param nuevo_valor Nueva lectura de temperatura.
 * @return Temperatura filtrada (promedio).
 */
float media_movil(float nuevo_valor) 
{
    static float buffer[TEMP_WINDOW_SIZE] = {0};
    static int indice = 0;
    static int contador = 0;

    buffer[indice] = nuevo_valor;
    indice = (indice + 1) % TEMP_WINDOW_SIZE;

    if(contador < TEMP_WINDOW_SIZE) contador++;

    float suma = 0;
    for(int i = 0; i < contador; i++) {
        suma += buffer[i];
    }

    return suma / contador;
}

/**
 * @brief Inicializa el ADC y selecciona el canal de entrada.
 *
 * @param input_channel Número de canal ADC a utilizar (0–3).
 */
void init_adc(uint8_t input_channel) 
{
    adc_init();
    adc_select_input(input_channel);  // Ej: canal 0 para GPIO 26
}
