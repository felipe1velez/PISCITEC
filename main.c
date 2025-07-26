/**
 * @file main.c
 * @brief Archivo principal del proyecto Pecera Pro (ambos proyectos).
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"

// Headers propios
#include "main.h"
#include "food.h"
#include "temperature.h"
#include "lights.h"

// I2C configuration for OLED display
#define I2C_PORT i2c1
#define I2C_SDA 6
#define I2C_SCL 7

// Global OLED display
ssd1306_t oled;

// ==== Variables Globales ====
volatile uint16_t top;
volatile uint16_t top_lights;
volatile float Temp = 0;
volatile float lights_value = 0.0f;
volatile float distance = 0.0f;
volatile int ir_value = 0;
volatile int vibration_value = 0;
volatile int vibration_count = 0;

volatile uint8_t flag_alarm = 0;
volatile uint8_t flag_low_food = 0;
volatile uint8_t flag_periodic = 0;
bool flag_echo = false;
bool flag_trigger = false;
bool flag_vibration = false;

volatile uint32_t echo_start = 0, echo_end = 0;
volatile bool trigger_ready = true;
bool rise_echo = false;
bool fall_echo = false;
float window[WINDOW_SIZE] = {0};
int wpos = 0, wcount = 0;

// ==== Prototipos ====
void oled_update_display(ssd1306_t *oled, float Temp, float lights_lux, float distance, int ir_value, int vibration_value);
int64_t apagar_buzzer(alarm_id_t id, void *user_data);

// ==== Main ====
int main() {
    stdio_init_all();

    // OLED Init
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    oled.external_vcc = false;
    if (!ssd1306_init(&oled, 128, 64, 0x3C, I2C_PORT)) {
        printf("Error al inicializar OLED\n");
    } else {
        ssd1306_clear(&oled);
        ssd1306_draw_string(&oled, 0, 0, 1, "OLED lista!");
        ssd1306_show(&oled);
    }

    // GPIO Init
    gpio_init(SERVO1_PIN);     gpio_set_dir(SERVO1_PIN, 1);
    gpio_init(LOW_FOOD_PIN);   gpio_set_dir(LOW_FOOD_PIN, 0);
    gpio_init(LED_PIN);        gpio_set_dir(LED_PIN, 1);
    gpio_init(HEATER_PIN);     gpio_set_dir(HEATER_PIN, 1);
    gpio_init(LIGHT_PIN);      gpio_set_dir(LIGHT_PIN, 1);
    gpio_set_pulls(LOW_FOOD_PIN, false, true);

    gpio_init(BUZZER_PIN);     gpio_set_dir(BUZZER_PIN, 1);
    gpio_put(BUZZER_PIN, 0); // buzzer off al inicio

    top = servo_pwm_init(SERVO1_PIN);
    food_control(SERVO1_PIN, FOOD_CLOSE, top);
    gpio_put(LED_PIN, 0);
    gpio_put(HEATER_PIN, 0);
    top_lights = pwm_init_basic(LIGHT_PIN);

    init_adc(TEMPERATURE_CHL);

    gpio_set_irq_enabled_with_callback(LOW_FOOD_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &irq_call_back);
    add_alarm_in_ms(LED_TIMEOUT_MS, come_back_irq1, NULL, true);

    struct repeating_timer periodic_timer;
    add_repeating_timer_ms(500, periodic_irq, NULL, &periodic_timer);

    gpio_init(TRIG_PIN); gpio_set_dir(TRIG_PIN, GPIO_OUT); gpio_put(TRIG_PIN, 0);
    gpio_init(ECHO_PIN); gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &irq_call_back);

    static repeating_timer_t timer;
    add_repeating_timer_ms(200, timer_callback, NULL, &timer);

    gpio_init(VIBRATION_PIN);
    gpio_set_dir(VIBRATION_PIN, GPIO_IN);
    gpio_pull_down(VIBRATION_PIN);
    gpio_set_irq_enabled_with_callback(VIBRATION_PIN, GPIO_IRQ_EDGE_RISE, true, &irq_call_back);

    // ==== Bucle Principal ====
    while (1) {
        if(flag_alarm == 1) {
            food_control(SERVO1_PIN, FOOD_OPEN, top);
            flag_alarm = 0;
            add_alarm_in_ms(LED_TIMEOUT_MS, come_back_irq2, NULL, true);
        }

        if(flag_alarm == 2) {
            food_control(SERVO1_PIN, FOOD_CLOSE, top);
            flag_alarm = 0;
            add_alarm_in_ms(LED_TIMEOUT_MS, come_back_irq1, NULL, true);
        }

        if(flag_low_food == 1) {
            gpio_put(LED_PIN, 1);
            ir_value = 1;
            flag_low_food = 0;
        }

        if(flag_low_food == 2) {
            gpio_put(LED_PIN, 0);
            ir_value = 0;
            flag_low_food = 0;
        }

        if(flag_periodic == 1) {
            Temp = temperature_control(HEATER_PIN);
            lights_value = lights_control(LIGHT_PIN, top_lights);
            flag_periodic = 0;

            // Conversión de luz a lux (ajusta este factor según tu sensor real)
            float lights_value_lux = lights_value * 0.122f;

            printf(" %.2f %.2f %.2f %d %d\n", Temp, lights_value, distance, ir_value, vibration_value);

            if(vibration_count > 0 && vibration_count <= 1) {
                vibration_value = 1;
                vibration_count++;
            } else {
                vibration_value = 0;
            }

            // Buzzer si hay vibración
            if(vibration_value == 1) {
                gpio_put(BUZZER_PIN, 1);
                add_alarm_in_ms(500, apagar_buzzer, NULL, true);
            }

            oled_update_display(&oled, Temp, lights_value_lux, distance, ir_value, vibration_value);
        }

        if(flag_trigger) {
            flag_trigger = false;
            if (trigger_ready) {
                trigger_ready = false;
                trigger_pulse();
            }
        }

        if(flag_echo) {
            flag_echo = false;
            if (rise_echo) {
                rise_echo = false;
                echo_start = time_us_32();
            } else if (fall_echo) {
                fall_echo = false;
                echo_end = time_us_32();
                uint32_t duracion = echo_end - echo_start;
                float distancia = duracion / 58.0f;
                if (distancia > 0 && distancia < 400) {
                    distance = moving_average(distancia);
                }
                trigger_ready = true;
            }
        }

        if(flag_vibration) {
            flag_vibration = false;
            vibration_count = 1;
        }
    }

    return 0;
}

// ==== Funciones Auxiliares ====
void irq_call_back(uint gpio, uint32_t events) {
    if (gpio == LOW_FOOD_PIN) {
        if (events & GPIO_IRQ_EDGE_RISE) flag_low_food = 1;
        if (events & GPIO_IRQ_EDGE_FALL) flag_low_food = 2;
    }
    if (gpio == ECHO_PIN) {
        if (events & GPIO_IRQ_EDGE_RISE) rise_echo = true;
        else if (events & GPIO_IRQ_EDGE_FALL) fall_echo = true;
        flag_echo = true;
    }
    if (gpio == VIBRATION_PIN && (events & GPIO_IRQ_EDGE_RISE)) {
        flag_vibration = true;
    }
}

int64_t come_back_irq1(alarm_id_t id, void *user_data) { flag_alarm = 1; return 0; }
int64_t come_back_irq2(alarm_id_t id, void *user_data) { flag_alarm = 2; return 0; }
int64_t apagar_buzzer(alarm_id_t id, void *user_data) { gpio_put(BUZZER_PIN, 0); return 0; }

bool periodic_irq(struct repeating_timer *t) {
    flag_periodic = 1;
    return true;
}

bool timer_callback(repeating_timer_t *rt) {
    flag_trigger = true;
    return true;
}

float moving_average(float new_value) {
    window[wpos] = new_value;
    wpos = (wpos + 1) % WINDOW_SIZE;
    if (wcount < WINDOW_SIZE) wcount++;
    float sum = 0;
    for (int i = 0; i < wcount; i++) sum += window[i];
    return sum / wcount;
}

void trigger_pulse(void) {
    gpio_put(TRIG_PIN, 1);
    for (volatile int i = 0; i < 150; i++) { __asm volatile("nop"); }
    gpio_put(TRIG_PIN, 0);
}

void oled_update_display(ssd1306_t *oled, float Temp, float lights_lux, float distance, int ir_value, int vibration_value) {
    char buffer[32];
    ssd1306_clear(oled);

    snprintf(buffer, sizeof(buffer), "Temp: %.1f C", Temp);
    ssd1306_draw_string(oled, 0, 0, 1, buffer);

    snprintf(buffer, sizeof(buffer), "Luz: %.1f lx", lights_lux);
    ssd1306_draw_string(oled, 0, 12, 1, buffer);

    snprintf(buffer, sizeof(buffer), "Dist: %.1f cm", distance);
    ssd1306_draw_string(oled, 0, 24, 1, buffer);

    snprintf(buffer, sizeof(buffer), "IR: %d", ir_value);
    ssd1306_draw_string(oled, 0, 36, 1, buffer);

    snprintf(buffer, sizeof(buffer), "Vibr: %d", vibration_value);
    ssd1306_draw_string(oled, 0, 48, 1, buffer);

    ssd1306_show(oled);
}
