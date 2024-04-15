#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "face.h"
#include "mono_vlsb.h"

const uint led_pin1 = 22;
const uint led_pin2 = 21;
const uint led_pin3 = 20;
const uint oled_sda = 14;
const uint oled_scl = 15;

void init_gpio();
void init_i2c();
ssd1306 init_display();
void blink_led(uint pin, uint64_t &lastToggleTime, uint interval);
void update_display(ssd1306 &display, mono_vlsb &bitmap_display, uint64_t currentTime);

int main() {
    stdio_init_all();
    init_gpio();
    init_i2c();
    ssd1306 display = init_display();

    // Create bitmap
    mono_vlsb bitmap_display(binary_data, 39, 39);

    uint64_t lastToggleTimeD1 = to_ms_since_boot(get_absolute_time());
    uint64_t lastToggleTimeD2 = lastToggleTimeD1;
    uint64_t lastToggleTimeD3 = lastToggleTimeD1;
    uint64_t lastPrintTime = lastToggleTimeD1;

    while (true) {
        uint64_t currentTime = to_ms_since_boot(get_absolute_time());

        // Blink LEDs with different frequencies
        blink_led(led_pin1, lastToggleTimeD1, 250); // Blink LED1 at 4Hz
        blink_led(led_pin2, lastToggleTimeD2, 500); // Blink LED2 at 2Hz
        blink_led(led_pin3, lastToggleTimeD3, 1000); // Blink LED3 at 1Hz

        // Update display every second
        if ((currentTime - lastPrintTime) >= 1000) {
            update_display(display, bitmap_display, currentTime);
            lastPrintTime = currentTime;
        }
    }
    return 0;
}

/*
 * void blink_led(uint pin, uint64_t &lastToggleTime, uint interval) {
    uint64_t currentTime = to_ms_since_boot(get_absolute_time());
    if ((currentTime - lastToggleTime) >= interval) {
        gpio_put(pin, !gpio_get(pin));
        lastToggleTime += interval / 2;
    }
}
 */

void blink_led(uint pin, uint64_t &lastToggleTime, uint interval) {
    uint64_t currentTime = to_ms_since_boot(get_absolute_time());
    if ((currentTime - lastToggleTime) >= interval) {
        uint32_t mask = 1ul << pin;
        gpio_xor_mask(mask);
        lastToggleTime += interval / 2;
    }
}

void update_display(ssd1306 &display, mono_vlsb &bitmap_display, uint64_t currentTime) {
    char timeMessage[32];
    printf("elapsed seconds: %llu\n", currentTime / 1000);
    sprintf(timeMessage, "Time: %ds", currentTime / 1000);
    display.fill(0);
    display.text(timeMessage, 0, 0);
    display.blit(bitmap_display, 0, 16);
    display.show();
}

void init_gpio() {
    gpio_init(led_pin1);
    gpio_set_dir(led_pin1, GPIO_OUT);
    gpio_init(led_pin2);
    gpio_set_dir(led_pin2, GPIO_OUT);
    gpio_init(led_pin3);
    gpio_set_dir(led_pin3, GPIO_OUT);
}

void init_i2c() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(oled_sda, GPIO_FUNC_I2C);
    gpio_set_function(oled_scl, GPIO_FUNC_I2C);
    gpio_pull_up(oled_sda);
    gpio_pull_up(oled_scl);
}

ssd1306 init_display() {
    ssd1306 display(i2c1, 0x3C, 128, 64);
    display.fill(0);
    return display;
}
