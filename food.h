/**
 * @file main.h
 * @brief This is a brief description of the food H file.
 *
 * Detailed description of the food C file.
 */
 
// Avoid duplication in code
#ifndef _FOOD_H_
#define _FOOD_H_

// Write your definitions and other macros here
#define FOOD_OPEN 0
#define FOOD_CLOSE 1
#define OPEN_MS 100
#define CLOSE_MS 5000

void food_control(uint8_t, uint8_t, float);
float angle_to_duty(float, float);
uint16_t servo_pwm_init(uint8_t);
// int64_t come_back_irq(alarm_id_t, void *);

#endif