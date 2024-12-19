#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "ldr.h"        // LDR driver
#include "ultrasonic.h" // Ultrasonic sensor driver
#include "servo.h"      // servo driver

// Pin definitions for LEDs and buzzer
#define BUZZER_PIN 18        // Pin for the buzzer
#define LDR_LED_PIN 15       // Pin for the LDR LED
#define LED_BLUE_PIN 5      // Pin for high temperature LED
#define LED_LOW_PIN 21       // Pin for low temperature LED


// Thresholds for sensors
#define DISTANCE_THRESHOLD 20.0 // Distance in cm to trigger the buzzer
#define LDR_THRESHOLD 2000     // ADC threshold for the LDR
#define TEMP_THRESHOLD 30.0    // Temperature threshold for LED control

// ADC input pin for internal temperature sensor
#define TEMP_SENSOR_ADC_CHANNEL 4

// Temperature calculation constants
#define TEMP_COEFFICIENT -1.721    // Temperature coefficient (in mV/°C)
#define TEMP_OFFSET 27.0           // Calibration temperature (°C) at 0.706V
#define V_REF 3.3                  // Reference voltage for the RP2040 (3.3V)

float read_temperature() {
    // Select ADC input for internal temperature sensor
    adc_select_input(TEMP_SENSOR_ADC_CHANNEL);
    uint16_t raw_adc = adc_read(); // Read the raw ADC value

    // Convert ADC value to voltage
    float voltage = raw_adc * V_REF / 4095.0;  // 12-bit ADC, range: 0-4095

    // Calculate the temperature in Celsius
    float temperature = TEMP_OFFSET - (voltage - 0.706) / TEMP_COEFFICIENT;
    return temperature;
}

int main() {
    // Initialize all peripherals
    stdio_init_all();
    init_ldr();
    init_ultrasonic();
    adc_init();
    servo_init(SERVO_PIN);

    // Initialize the buzzer
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);

    // Initialize the LED for LDR
    gpio_init(LDR_LED_PIN);
    gpio_set_dir(LDR_LED_PIN, GPIO_OUT);
    gpio_put(LDR_LED_PIN, 0);

    // Initialize the temperature LEDs
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_BLUE_PIN, 0); // Initially off

    gpio_init(LED_LOW_PIN);
    gpio_set_dir(LED_LOW_PIN, GPIO_OUT);
    gpio_put(LED_LOW_PIN, 0); // Initially off

    int temp_read_count = 0; // Counter for temperature readings

    bool object_detected = false; // Track object presence
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);

    while (true) {
        // --- LDR Sensor ---
        uint16_t ldr_value = read_ldr_value();
        printf("LDR Value: %d\n", ldr_value);

        if (ldr_value < LDR_THRESHOLD) {
            gpio_put(LDR_LED_PIN, 1); // Turn on LED if brightness is low
        } else {
            gpio_put(LDR_LED_PIN, 0); // Turn off LED if brightness is high
        }

        // --- Ultrasonic Sensor ---
        float distance = measure_distance();
        printf("Distance: %.2f cm\n", distance);

        if (distance > 0 && distance < DISTANCE_THRESHOLD) {
            if (!object_detected) { // Object detected for the first time
                printf("Object detected: Rotating servo to 180°\n");
                set_servo_angle(slice_num, 360);
                gpio_put(BUZZER_PIN, 1); // Turn on buzzer
                object_detected = true;
            }
        } else {
            if (object_detected) { // Object removed
                printf("Object removed: Rotating servo to 0°\n");
                set_servo_angle(slice_num, 180);
                gpio_put(BUZZER_PIN, 0); // Turn off buzzer
                object_detected = false;
            }
        }

        // --- Temperature Sensor ---
        float temperature = read_temperature();
        temp_read_count++; // Increment the temperature read count
        printf("Temperature: %.2f°C | Reading #%d\n", temperature, temp_read_count);

        if (temperature < TEMP_THRESHOLD) {
            gpio_put(LED_BLUE_PIN, 1); // Turn on high-temperature LED
            gpio_put(LED_LOW_PIN, 0);  // Turn off low-temperature LED
        } else {
            gpio_put(LED_BLUE_PIN, 0); // Turn off high-temperature LED
            gpio_put(LED_LOW_PIN, 1);  // Turn on low-temperature LED
        }

        sleep_ms(500); // Delay for both sensors
    }
}
