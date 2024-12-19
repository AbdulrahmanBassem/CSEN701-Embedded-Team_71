#ifndef LDR_H
#define LDR_H

#include "pico/stdlib.h"

// Define the ADC pin for LDR
#define LDR_ADC_PIN 26  // ADC0 pin

// Function prototypes
void init_ldr();           // Initialize the LDR
uint16_t read_ldr_value(); // Read the LDR value (0-4095 for RP2040 ADC)

#endif  // LDR_H
