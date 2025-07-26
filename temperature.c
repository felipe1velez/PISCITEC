/**
 * @file temperature.c
 * @brief This is the temperature file of the Pecera Pro project.
 * Program.
 */

// Standard libraries
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "temperature.h"

bool heater_on = false;

// Lee el ADC y lo convierte a voltaje
float read_temperature() 
{
    adc_select_input(0);
    uint16_t raw = adc_read();              // Lectura cruda (12 bits)
    return (raw * 3.3f / 4095)*100;               // Conversión a grados
}

// Check temperature and take action if necessary
float temperature_control(uint8_t gpio_h)
{
    float temp = media_movil(read_temperature());
    if(temp > HOT_TEMPERATURE) 
    {
       // printf("Temperatura alta: %.1f°C\n", temp);
        // Apagar el calentador si está encendido
        if(heater_on) 
        {
            //printf("Apagando calentador.\n");
            gpio_put(gpio_h, 0);
            heater_on = false;
        }
    } 
    else if(temp < COLD_TEMPERATURE) 
    {
        //printf("Temperatura baja: %.1f°C\n", temp);
        // Encender el calentador si está apagado
        if(!heater_on) 
        {
            //printf("Encendiendo calentador.\n");
            gpio_put(gpio_h, 1);
            heater_on = true;
        }
        
    } 
    else 
    {
        //printf("Temperatura normal: %.1f°C", temp);
        //printf(" con calentador %s\n", heater_on ? "encendido" : "apagado");
    }
    return temp;
}

// Media móvil para suavizar lecturas
float media_movil(float nuevo_valor) 
{
    static float buffer[TEMP_WINDOW_SIZE] = {0};
    static int indice = 0;
    static int contador = 0;

    buffer[indice] = nuevo_valor;
    indice = (indice + 1) % TEMP_WINDOW_SIZE;

    if(contador < TEMP_WINDOW_SIZE){ contador++; }

    float suma = 0;
    for(int i = 0; i < contador; i++)
    { suma += buffer[i]; }

    return suma / contador;
}

// Inicializa el ADC y selecciona el canal deseado
void init_adc(uint8_t input_channel) 
{
    adc_init();
    adc_select_input(input_channel);  // 0 para GPIO 26, etc.
}