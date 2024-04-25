#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

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

    gpio_init(oled_sda);
    gpio_init(oled_scl);

    // Initialize I2C bus and SSD1306 display
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(oled_sda, GPIO_FUNC_I2C);
    gpio_set_function(oled_scl, GPIO_FUNC_I2C);
    gpio_pull_up(oled_sda);
    gpio_pull_up(oled_scl);





    while (true) {


    }
}