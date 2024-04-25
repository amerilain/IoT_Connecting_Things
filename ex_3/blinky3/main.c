#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

const uint led_pin1 = 22;
const uint led_pin2 = 21;
const uint led_pin3 = 20;

int main() {
    stdio_init_all();

    gpio_init(led_pin1);
    gpio_init(led_pin2);
    gpio_init(led_pin3);

    gpio_set_dir(led_pin1, GPIO_OUT);
    gpio_set_dir(led_pin2, GPIO_OUT);
    gpio_set_dir(led_pin3, GPIO_OUT);

    uint64_t lastToggleTimeD1 = to_ms_since_boot(get_absolute_time());
    uint64_t lastToggleTimeD2 = lastToggleTimeD1;
    uint64_t lastToggleTimeD3 = lastToggleTimeD1;
    uint64_t lastPrintTime = lastToggleTimeD1;

    while (1) {
        uint64_t currentTime = to_ms_since_boot(get_absolute_time());

        // Toggle LED D1
        if ((currentTime - lastToggleTimeD1) >= 125) {
            gpio_put(led_pin1, !gpio_get(led_pin1));
            lastToggleTimeD1 += 125;
        }

        // Toggle LED D2
        if ((currentTime - lastToggleTimeD2) >= 250) {
            gpio_put(led_pin2, !gpio_get(led_pin2));
            lastToggleTimeD2 += 250;
        }

        // Toggle LED D3
        if ((currentTime - lastToggleTimeD3) >= 500) {
            gpio_put(led_pin3, !gpio_get(led_pin3));
            lastToggleTimeD3 += 500;
        }

        // Print elapsed seconds to the terminal
        if ((currentTime - lastPrintTime) >= 1000) {
            printf("Elapsed seconds: %llu\n", currentTime / 1000);
            lastPrintTime += 1000;
        }

        sleep_ms(10);
    }

    return 0;
}
