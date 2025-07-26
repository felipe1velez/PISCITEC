/**
 * @file ldr_sensor.c
 * @brief Implementación del sensor LDR para medición de luz ambiente.
 *
 * Este módulo configura un canal ADC para leer valores analógicos provenientes
 * de un sensor LDR conectado a uno de los pines GPIO del Raspberry Pi Pico.
 * La lectura es usada para ajustar el control de iluminación en el sistema Pecera Pro.
 *
 * @author
 * Duván Felipe Vélez Restrepo
 * @date 2025
 */

#include "ldr_sensor.h"
#include "hardware/adc.h"

static uint g_adc_channel;  ///< Canal ADC global usado para la lectura del LDR

/**
 * @brief Inicializa el canal ADC asociado al sensor LDR.
 *
 * Realiza la inicialización del ADC y configura el pin GPIO correspondiente 
 * al canal especificado (ej. canal 0 → GPIO26).
 *
 * @param adc_channel Canal del ADC a utilizar (0 para GPIO26, 1 para GPIO27, etc.)
 */
void ldr_init(uint adc_channel) {
    adc_init();
    g_adc_channel = adc_channel;
    adc_gpio_init(26 + adc_channel); // 0 → GPIO26, 1 → GPIO27, etc.
    adc_select_input(g_adc_channel);
}

/**
 * @brief Realiza una lectura desde el canal ADC configurado.
 *
 * @return Valor crudo de 12 bits (rango 0–4095) proporcional al nivel de luz.
 */
uint16_t ldr_read() {
    adc_select_input(g_adc_channel);
    return adc_read();  // 12-bit value (0-4095)
}

