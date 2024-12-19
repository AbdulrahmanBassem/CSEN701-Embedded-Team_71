#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "pico/stdlib.h"

// Define HC-SR04 pins
#define TRIGGER_PIN 16
#define ECHO_PIN    17

// Function declarations
void init_ultrasonic();
float measure_distance();

#endif // ULTRASONIC_H
