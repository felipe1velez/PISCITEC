/**
 * @file lights.c
 * @brief Control de brillo de iluminación en base a luz ambiente con media móvil y PWM.
 *
 * Este archivo implementa las funciones necesarias para leer un sensor de luz (fotocelda),
 * aplicar una media móvil para estabilizar la señal y ajustar el brillo de una fuente
 * de luz mediante modulación PWM. Se utiliza el canal ADC 1 (GPIO27) para la lectura del sensor.
 * 
 * El duty cycle se adapta en tiempo real según la cantidad de luz ambiente detectada.
 * PWM configurado a 10 kHz para evitar parpadeos perceptibles.
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
#include "hardware/adc.h"

#include "lights.h"

/**
 * @brief Lee el nivel de luz desde el canal ADC 1 (GPIO27).
 *
 * Selecciona el canal adecuado y retorna el valor crudo de 12 bits
 * proporcionado por el convertidor analógico-digital.
 *
 * @return Valor de ADC (0 a 4095).
 */
uint16_t read_lights() 
{
    adc_select_input(1);  // Canal 1 = GPIO27
    return adc_read();    // 0–4095 (12 bits)
}

/**
 * @brief Ajusta el duty cycle del PWM basado en la lectura del sensor de luz.
 *
 * Utiliza una tabla de umbrales para determinar cuánta potencia debe aplicarse
 * a la fuente de luz en función del nivel de iluminación ambiental. A menor luz,
 * mayor intensidad (duty) aplicada.
 *
 * @param gpio_h Pin GPIO al que está conectada la salida PWM.
 * @param top Valor máximo del contador PWM (frecuencia base).
 * @return Valor de luz promediado tras filtrado (media móvil).
 */
float lights_control(uint8_t gpio_h, uint16_t top)
{
    uint16_t level = media_movil2(read_lights());
    uint32_t duty = 0;
    
    if (level < 500)
        duty = top * 1.0;   // 100%
    else if (level < 600)
        duty = top * 0.8;   // 80%
    else if (level < 800)
        duty = top * 0.5;   // 60%
    else if (level < 1100)
        duty = top * 0.3;   // 50%
    else if (level < 1600)
        duty = top * 0.1;   // 30%
    else
        duty = 0;           // Luz alta → apagar
    
    pwm_set_gpio_level(gpio_h, duty);
    return level;
}

/**
 * @brief Aplica un filtro de media móvil de ventana fija para suavizar la lectura de luz.
 *
 * Esta función reduce el ruido y fluctuaciones rápidas que pueden causar parpadeos no deseados
 * en la fuente de iluminación. La ventana utilizada es de 10 muestras.
 *
 * @param nuevo_valor Nueva lectura del sensor de luz (ADC).
 * @return Valor suavizado con media móvil.
 */
float media_movil2(float nuevo_valor) 
{
    static float buffer[10] = {0};
    static int indice = 0;
    static int contador = 0;

    buffer[indice] = nuevo_valor;
    indice = (indice + 1) % 10;

    if (contador < 10) contador++;

    float suma = 0;
    for (int i = 0; i < contador; i++) {
        suma += buffer[i];
    }

    return suma / contador;
}

/**
 * @brief Inicializa la señal PWM en un GPIO con frecuencia de ~10 kHz.
 *
 * Configura el pin especificado como salida PWM y calcula el valor de "top"
 * correspondiente a la frecuencia objetivo, usando el reloj del sistema.
 *
 * @param gpio Número del pin GPIO a configurar como PWM.
 * @return Valor de 'top' calculado para esa frecuencia.
 */
uint16_t pwm_init_basic(uint8_t gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);

    uint64_t clockspeed = clock_get_hz(clk_sys);
    uint16_t top = clockspeed / 10000;  // PWM de ~10kHz

    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, top);

    pwm_init(slice_num, &config, true);
    pwm_set_enabled(slice_num, true);

    return top;
}
