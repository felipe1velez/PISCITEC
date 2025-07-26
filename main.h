/**
 * @file main.h
 * @brief Header principal del proyecto Pecera Pro.
 *
 * Contiene definiciones de pines, constantes, y prototipos de funciones para main.c.
 */

#ifndef _MAIN_H_
#define _MAIN_H_

// ==== Definiciones de Pines ====
#define SERVO1_PIN      15
#define LOW_FOOD_PIN    16
#define LED_PIN         17
#define HEATER_PIN      18
#define LIGHT_PIN       19
#define BUZZER_PIN 11


#define TRIG_PIN        2   // Trigger del sensor ultrasónico
#define ECHO_PIN        3   // Echo del sensor ultrasónico
#define VIBRATION_PIN   4   // Sensor de vibración

// ==== Constantes de Control ====
#define TEMPERATURE_CHL     0       // Canal ADC para LM35
#define LED_TIMEOUT_MS      3000    // Duración del LED de comida (ms)
#define WINDOW_SIZE         5       // Tamaño de ventana para promedio móvil

// ==== Prototipos de Funciones ====
void irq_call_back(uint gpio, uint32_t events);
int64_t come_back_irq1(alarm_id_t id, void *user_data);
int64_t come_back_irq2(alarm_id_t id, void *user_data);
bool periodic_irq(struct repeating_timer *t);
bool timer_callback(repeating_timer_t *rt);
float moving_average(float new_value);
void trigger_pulse(void);

#endif // _MAIN_H_
