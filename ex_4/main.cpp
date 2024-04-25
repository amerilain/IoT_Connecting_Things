#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include <sstream>

const uint LED_PIN = 22;
const uint BUTTON_PIN = 9;
const uint LED_D1_PIN = 22;
const uint LED_D2_PIN = 21;
const uint LED_D3_PIN = 20;

const char* MQTT_BROKER_ADDRESS = "18.198.188.151";
const int MQTT_BROKER_PORT = 21883;
const char* MQTT_CLIENT_ID = "PicoW-sample";
const char* MQTT_USERNAME = "SmartIoT";
const char* MQTT_PASSWORD = "SmartIoTMQTT";
const char* SEND_TOPIC = "Yo/Mama";
const char* RECEIVE_TOPIC = "Yo/LEDs";

int msg_count = 0;

void messageArrived(MQTT::MessageData &md);
void setup_gpio();
MQTT::Client<IPStack, Countdown> setup_mqtt(IPStack &ipstack);

int main() {
    stdio_init_all();
    setup_gpio();

    IPStack ipstack("Aleksi", "123fffff");
    auto client = setup_mqtt(ipstack);

    while (true) {
        // Publish a message when the button is pressed
        if (!gpio_get(BUTTON_PIN)) { // Button is active low
            char buf[100];
            sprintf(buf, R"({"topic":"%s", "msg":"Button pressed, count: %d"})", SEND_TOPIC, ++msg_count);
            MQTT::Message message;
            message.qos = MQTT::QOS0;
            message.retained = false;
            message.dup = false;
            message.payload = (void *)buf;
            message.payloadlen = strlen(buf) + 1;

            client.publish(SEND_TOPIC, message);
            printf("Sent message: %s\n", buf);

            sleep_ms(1000); // Debounce delay
        }

        client.yield(10); // Process incoming MQTT messages and maintain connection
    }
}


void messageArrived(MQTT::MessageData &md) {
    const char* topicName = md.topicName.lenstring.data;
    const char* payload = (char *)md.message.payload;
    int payloadLength = md.message.payloadlen;

    printf("Message arrived on topic: %.*s, Content: %.*s\n",
           md.topicName.lenstring.len, topicName, payloadLength, payload);

    // Convert payload to string
    std::string messageContent(payload, payloadLength);

    // Tokenize the message
    std::vector<std::string> tokens;
    std::istringstream tokenStream(messageContent);
    std::string token;

    while (getline(tokenStream, token, ';')) {
        tokens.push_back(token);
    }

    if (std::string(topicName, md.topicName.lenstring.len) == "Yo/LEDs") {
        if (tokens.size() < 2) {
            printf("Invalid message format on 'Yo/LEDs': not enough data parts.\n");
            return;
        }

        std::string led = tokens[0];
        std::string command = tokens[1];

        // Determine which LED to control
        uint led_pin = 0;
        if (led == "D1") {
            led_pin = LED_D1_PIN;
        } else if (led == "D2") {
            led_pin = LED_D2_PIN;
        } else if (led == "D3") {
            led_pin = LED_D3_PIN;
        } else {
            printf("Invalid LED identifier: %s\n", led.c_str());
            return;
        }

        // Control LED based on command
        if (command == "ON") {
            gpio_put(led_pin, 1);
        } else if (command == "OFF") {
            gpio_put(led_pin, 0);
        } else if (command == "TOGG") {
            gpio_put(led_pin, !gpio_get(led_pin));
        } else {
            printf("Invalid command: %s\n", command.c_str());
        }
    }
}


void setup_gpio() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Ensure LED is off initially

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN); // Use internal pull-up resistor

    // Initialize and turn off all LED pins
    const uint leds[] = { LED_D1_PIN, LED_D2_PIN, LED_D3_PIN };
    for (uint pin : leds) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_put(pin, 0);
    }
}

// Function to setup and connect to MQTT broker
MQTT::Client<IPStack, Countdown> setup_mqtt(IPStack &ipstack) {
    printf("Connecting to MQTT broker at %s:%d\n", MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT);
    ipstack.connect(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT);

    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    connectData.MQTTVersion = 3;
    connectData.clientID.cstring = (char *)MQTT_CLIENT_ID;
    connectData.username.cstring = (char *)MQTT_USERNAME;
    connectData.password.cstring = (char *)MQTT_PASSWORD;

    auto client = MQTT::Client<IPStack, Countdown>(ipstack);
    client.connect(connectData);

    client.subscribe(SEND_TOPIC, MQTT::QOS2, messageArrived);
    client.subscribe(RECEIVE_TOPIC, MQTT::QOS2, messageArrived);
    printf("Subscribed to MQTT topics %s and %s\n", SEND_TOPIC, RECEIVE_TOPIC);

    return client;
}

