#include "ultrasonic.h"

void init_ultrasonic() {
    // Initialize trigger pin as output
    gpio_init(TRIGGER_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_put(TRIGGER_PIN, 0); // Ensure it's low

    // Initialize echo pin as input
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

float measure_distance() {
    // Send a 10us pulse to trigger pin
    gpio_put(TRIGGER_PIN, 1);
    sleep_us(10);
    gpio_put(TRIGGER_PIN, 0);

    // Measure the time for echo to return
    absolute_time_t start_time = get_absolute_time();
    while (!gpio_get(ECHO_PIN)) { // Wait for the echo pin to go HIGH
        if (absolute_time_diff_us(start_time, get_absolute_time()) > 20000) {
            return -1; // Timeout (no object detected)
        }
    }

    start_time = get_absolute_time();
    while (gpio_get(ECHO_PIN)) { // Measure the HIGH time of the echo signal
        if (absolute_time_diff_us(start_time, get_absolute_time()) > 20000) {
            return -1; // Timeout (object too far)
        }
    }

    // Calculate pulse duration
    int64_t pulse_duration = absolute_time_diff_us(start_time, get_absolute_time());

    // Calculate distance (speed of sound = 343 m/s or 34300 cm/s)
    float distance = pulse_duration * 0.0343 / 2.0; // Divide by 2 for round-trip
    return distance;
}
