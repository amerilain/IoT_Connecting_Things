#include <stdio.h>
#include <string.h>
#include <cmath>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "ssd1306.h"

const uint LED_PIN = 22;
const uint BUTTON_PIN = 9;
const uint LED_D1_PIN = 22;
const uint LED_D2_PIN = 21;
const uint LED_D3_PIN = 20;

#define USE_MQTT
#define USE_SSD1306


#ifdef USE_SSD1306
static const uint8_t raspberry26x32[] =
        {0x0, 0x0, 0xe, 0x7e, 0xfe, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xfc, 0xfe,
         0xfe, 0xff, 0xff,0xff, 0xff, 0xff, 0xfe, 0x7e,
         0x1e, 0x0, 0x0, 0x0, 0x80, 0xe0, 0xf8, 0xfd,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd,
         0xf8, 0xe0, 0x80, 0x0, 0x0, 0x1e, 0x7f, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0x7f, 0x1e, 0x0, 0x0,
         0x0, 0x3, 0x7, 0xf, 0x1f, 0x1f, 0x3f, 0x3f,
         0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f,
         0x3f, 0x1f, 0x1f, 0xf, 0x7, 0x3, 0x0, 0x0 };
#endif

static const char *send_topic = "Yo/Mama";
static const char *receive_topic = "Yo/LEDs";
int msg_count = 0;

void messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    std::string str((char *) message.payload, message.payloadlen);
    printf("%s\n", str.c_str()); // Modified line


    // Parse the message
    std::string led = str.substr(0, 2);
    std::string command = str.substr(3);

    uint led_pin = 0;

    if (led == "D1") {
        led_pin = LED_D1_PIN;
    } else if (led == "D2") {
        led_pin = LED_D2_PIN;
    } else if (led == "D3") {
        led_pin = LED_D3_PIN;
    }

    // Adjust the state of the LED
    if (command == "ON") {
        // Turn on the LED
        gpio_put(led_pin, 1);
    } else if (command == "OFF") {
        // Turn off the LED
        gpio_put(led_pin, 0);
    } else if (command == "TOGG") {
        // Toggle the LED
        gpio_put(led_pin, !gpio_get(led_pin));
    }
}

int main() {

    stdio_init_all();
    printf("\nBoot\n");

    // Initialize LED pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    gpio_init(LED_D1_PIN);
    gpio_set_dir(LED_D1_PIN, GPIO_OUT);
    gpio_put(LED_D1_PIN, 0); // Turn off initially

    gpio_init(LED_D2_PIN);
    gpio_set_dir(LED_D2_PIN, GPIO_OUT);
    gpio_put(LED_D2_PIN, 0); // Turn off initially

    gpio_init(LED_D3_PIN);
    gpio_set_dir(LED_D3_PIN, GPIO_OUT);
    gpio_put(LED_D3_PIN, 0); // Turn off initially


#ifdef USE_SSD1306
    // I2C is "open drain",
    // pull ups to keep signal high when no data is being sent
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); // the display has external pull-ups
    gpio_set_function(15, GPIO_FUNC_I2C); // the display has external pull-ups
    ssd1306 display(i2c1);
    display.fill(0);
    display.text("Hello", 0, 0);
    mono_vlsb rb(raspberry26x32, 26, 32);
    display.blit(rb, 20, 20);
    display.rect(15, 15, 35, 45, 1);
    display.line(60, 5, 120, 60, 1);
    display.line(60, 60, 120, 5, 1);
    display.show();
#if 1
    for (int i = 0; i < 128; ++i) {
        sleep_ms(50);
        display.scroll(1, 0);
        display.show();
    }
    display.text("Done", 20, 20);
    display.show();
#endif

#endif


#ifdef USE_MQTT
    //IPStack ipstack("SSID", "PASSWORD"); // example
    //IPStack ipstack("KME662", "SmartIot"); // example
    IPStack ipstack("Aleksi", "123fffff"); // example
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);

    int rc = ipstack.connect("18.198.188.151", 21883);
    //int rc = ipstack.connect("localhost", 1883);
    if (rc != 1) {
        printf("rc from TCP connect is %d\n", rc);
    }

    printf("MQTT connecting\n");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *) "PicoW-sample";
    data.username.cstring = (char *) "SmartIoT";
    data.password.cstring = (char *) "SmartIoTMQTT";

    rc = client.connect(data);
    if (rc != 0) {
        printf("rc from MQTT connect is %d\n", rc);
        while (true) {
            tight_loop_contents();
        }
    }
    printf("MQTT connected\n");

    // We subscribe QoS2. Messages sent with lower QoS will be delivered using the QoS they were sent with
    rc = client.subscribe(send_topic, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("rc from MQTT subscribe is %d\n", rc);
    }

    rc = client.subscribe(receive_topic, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("rc from MQTT subscribe is %d\n", rc);
    } else {
        printf("Subscribed to topic: %s\n", receive_topic);
    }

    printf("MQTT subscribed\n");

    auto mqtt_send = make_timeout_time_ms(2000);
    int mqtt_qos = 0;
#endif


    while (true) {
        if (!gpio_get(BUTTON_PIN)) { // Check if button is pressed (active low)
            char buf[100];
            sprintf(buf, R"({"topic":"%s", "msg":"Button pressed, count: %d"})", send_topic, ++msg_count);
            MQTT::Message message;
            message.qos = MQTT::QOS0;
            message.retained = false;
            message.dup = false;
            message.payload = (void *)buf;
            message.payloadlen = strlen(buf) + 1;

            if (client.publish(send_topic, message) != 0) {
                printf("Failed to publish message\n");
            } else {
                // Print the message sent to the serial monitor
                printf("Sent message on %s: %s\n", send_topic, buf);
            }

            gpio_put(LED_PIN, 1); // Turn on LED to indicate message sent
            sleep_ms(200);
            gpio_put(LED_PIN, 0); // Turn off LED

            sleep_ms(1000); // Debounce delay
            while (!gpio_get(BUTTON_PIN)) { // Wait for button release
                tight_loop_contents();
            }
        }
        client.yield(10); // Process incoming messages and maintain connection
    }
}