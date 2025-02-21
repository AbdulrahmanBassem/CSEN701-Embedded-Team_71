#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo.h"

static uint slice_num_global; // Store slice number for the servo

// Initialize the servo motor
void servo_init(uint gpio_pin) {
    // Set up GPIO for PWM
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM); // Set GPIO pin as PWM
    slice_num_global = pwm_gpio_to_slice_num(gpio_pin); // Get PWM slice

    // Set PWM frequency to 50Hz (20ms period)
    pwm_set_wrap(slice_num_global, 20000); // 20,000 ticks for 50Hz frequency
    pwm_set_clkdiv(slice_num_global, 64.0f); // Set clock divider
    pwm_set_enabled(slice_num_global, true); // Enable PWM on the slice

    printf("Servo Motor Initialized on GPIO %d\n", gpio_pin);
}

// Set the servo angle (0° to 180°)
void set_servo_angle(uint slice_num, uint angle) {
    uint duty = 500 + (angle * 2000 / 180);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, duty);
    printf("Setting angle: %d°, Duty cycle: %d\n", angle, duty);
}