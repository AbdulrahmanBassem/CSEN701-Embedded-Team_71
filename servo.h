#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#define SERVO_PIN 28

// Function prototypes
void servo_init(uint gpio_pin);
void set_servo_angle(uint slice_num, uint angle);

#endif // SERVO_H