//
// Created by iamna on 01/03/2024.
//

#ifndef PICO_MODBUS_I2C_DISPLAY_H
#define PICO_MODBUS_I2C_DISPLAY_H
#include "ssd1306.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

class I2C_Display {
public:

    I2C_Display(uint8_t sda, uint8_t scl, i2c_inst_t *i2c);

    void displayText(const char *text);

    void clearDisplay();

    void drawFilledRect(int16_t x, int16_t y, int16_t width, int16_t height, bool fill);

private:
    std::shared_ptr<ssd1306> display;
};
#endif //PICO_MODBUS_I2C_DISPLAY_H
