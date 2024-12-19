#include "ldr.h"
#include "hardware/adc.h"

// Initialize the LDR (configure ADC)
void init_ldr() {
    adc_init();
    adc_gpio_init(LDR_ADC_PIN); // Initialize the ADC pin
}

// Read the LDR value
uint16_t read_ldr_value() {
    adc_select_input(0); // Explicitly select the ADC0 channel (assuming LDR is on channel 0)
    return adc_read();   // Returns a value between 0 (low light) to 4095 (bright light)
}
