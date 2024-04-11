#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "i2c_display.h"

const uint led_pin1 = 22;
const uint led_pin2 = 21;
const uint led_pin3 = 20;

const uint oled_sda = 14;
const uint oled_scl = 15;

int main() {
    stdio_init_all();
    gpio_init(led_pin1);
    gpio_init(led_pin2);
    gpio_init(led_pin3);
    gpio_set_dir(led_pin1, GPIO_OUT);
    gpio_set_dir(led_pin2, GPIO_OUT);
    gpio_set_dir(led_pin3, GPIO_OUT);
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(oled_sda, GPIO_FUNC_I2C);
    gpio_set_function(oled_scl, GPIO_FUNC_I2C);
    gpio_pull_up(oled_sda);
    gpio_pull_up(oled_scl);
    I2C_Display oledDisplay{oled_sda, oled_scl, i2c1};

    uint64_t lastToggleTimeD1 = to_ms_since_boot(get_absolute_time());
    uint64_t lastToggleTimeD2 = lastToggleTimeD1;
    uint64_t lastToggleTimeD3 = lastToggleTimeD1;
    uint64_t lastPrintTime = lastToggleTimeD1;

    while (1) {
        uint64_t currentTime = to_ms_since_boot(get_absolute_time());

        // Blink LED1 at 4Hz
        if ((currentTime - lastToggleTimeD1) >= 250) {
            gpio_put(led_pin1, !gpio_get(led_pin1));
            lastToggleTimeD1 += 125;
        }

        // Blink LED2 at 2Hz
        if ((currentTime - lastToggleTimeD2) >= 500) {
            gpio_put(led_pin2, !gpio_get(led_pin2));
            lastToggleTimeD2 += 250;
        }

        // Blink LED3 at 1Hz
        if ((currentTime - lastToggleTimeD3) >= 1000) {
            gpio_put(led_pin3, !gpio_get(led_pin3));
            lastToggleTimeD3 += 500;
        }

        // Print elapsed seconds on OLED
        if ((currentTime - lastPrintTime) >= 1000) {
            char timeMessage[20];
            sprintf(timeMessage, "Time: %llu s", currentTime / 1000);

            // Clear the display
            oledDisplay.clearDisplay();

            // Display time at the top left of the screen
            oledDisplay.displayText(timeMessage);

            // Display filled box
            oledDisplay.drawFilledRect(0, 10, 39, 39, true);

            lastPrintTime += 1000;
        }
    }
    return 0;
}