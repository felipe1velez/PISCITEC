/**
 * @file main.h
 * @brief Header principal del proyecto Piscitec (Pecera Pro).
 *
 * Contiene definiciones de pines, constantes globales y prototipos de funciones
 * utilizadas en el archivo `main.c` para el control principal del sistema embebido.
 * Estas funciones cubren manejo de interrupciones, timers y lógica de medición
 * con sensores ultrasónicos y de vibración.
 *
 * @author
 * Duván Felipe Vélez Restrepo
 * @date 2025
 */

#ifndef _MAIN_H_
#define _MAIN_H_

// ==== Definiciones de Pines ====

/// GPIO del servomotor que acciona el dispensador de comida
#define SERVO1_PIN      15

/// GPIO de entrada del sensor IR que detecta si hay comida
#define LOW_FOOD_PIN    16

/// GPIO del LED indicador de estado
#define LED_PIN         17

/// GPIO del calentador de agua
#define HEATER_PIN      18

/// GPIO de la tira LED (iluminación)
#define LIGHT_PIN       19

/// GPIO del buzzer para alertas sonoras
#define BUZZER_PIN      11

/// GPIO del trigger del sensor ultrasónico (nivel de agua)
#define TRIG_PIN        2

/// GPIO del echo del sensor ultrasónico (nivel de agua)
#define ECHO_PIN        3

/// GPIO del sensor de vibración (eventos físicos)
#define VIBRATION_PIN   4

// ==== Constantes de Control ====

/// Canal ADC utilizado por el sensor LM35
#define TEMPERATURE_CHL     0

/// Tiempo de encendido del LED tras la activación del servomotor (en ms)
#define LED_TIMEOUT_MS      3000

/// Tamaño de la ventana para aplicar promedio móvil al sensor ultrasónico
#define WINDOW_SIZE         5

// ==== Prototipos de Funciones ====

/**
 * @brief Manejador general de interrupciones para pines GPIO.
 *
 * Gestiona los eventos de los sensores IR, ultrasónico y de vibración.
 *
 * @param gpio Número de GPIO que generó la interrupción.
 * @param events Máscara de evento (flanco de subida/bajada).
 */
void irq_call_back(uint gpio, uint32_t events);

/**
 * @brief Callback que genera la apertura del dispensador de comida.
 *
 * Asociado a un temporizador de retardo.
 */
int64_t come_back_irq1(alarm_id_t id, void *user_data);

/**
 * @brief Callback que genera el cierre del dispensador de comida.
 *
 * Asociado a un temporizador de retardo.
 */
int64_t come_back_irq2(alarm_id_t id, void *user_data);

/**
 * @brief Timer periódico que activa la lectura de sensores cada cierto intervalo.
 *
 * @param t Puntero a la estructura del temporizador.
 * @return true para repetir el temporizador.
 */
bool periodic_irq(struct repeating_timer *t);

/**
 * @brief Timer que controla el disparo del sensor ultrasónico por tiempo.
 *
 * @param rt Puntero al temporizador.
 * @return true para repetir el evento.
 */
bool timer_callback(repeating_timer_t *rt);

/**
 * @brief Aplica promedio móvil sobre las lecturas del sensor ultrasónico.
 *
 * @param new_value Nueva lectura de distancia.
 * @return Valor suavizado.
 */
float moving_average(float new_value);

/**
 * @brief Genera un pulso de disparo al sensor ultrasónico (trigger).
 */
void trigger_pulse(void);

#endif // _MAIN_H_
