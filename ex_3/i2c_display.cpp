#include "i2c_display.h"

I2C_Display::I2C_Display(uint8_t sda, uint8_t scl, i2c_inst_t *i2c) {
    i2c_init(i2c, 100000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);
    display = std::make_shared<ssd1306>(i2c);
}

void I2C_Display::clearDisplay() {
    display->fill(0);
}

void I2C_Display::displayText(const char *text) {
    display->text(text, 0, 0);
    display->show();
}

void I2C_Display::drawFilledRect(int16_t x, int16_t y, int16_t width, int16_t height, bool fill) {
    display->rect(x, y, width, height, 1, fill);
    display->show();
}

void I2C_Display::drawPixel(int16_t x, int16_t y) {
    drawFilledRect(x, y, 1, 1, true);
}
