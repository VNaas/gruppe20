#include "BRTT.h"

#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00

void testA()
{
    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT | GPIO_PULL_UP);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    int testA = 1;
    while (1)
    {
        while (gpio_get_pin_value(TEST_A))
        {
            gpio_set_pin_low(RESPONSE_A);
            testA = 0;
            while (!testA)
            {
                testA = gpio_get_pin_value(TEST_A);
            }
            gpio_set_pin_high(RESPONSE_A);
        }
    }
}

void testAll(){
    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT | GPIO_PULL_UP);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(TEST_B, GPIO_DIR_INPUT | GPIO_PULL_UP);
    gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(TEST_C, GPIO_DIR_INPUT | GPIO_PULL_UP);
    gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    int testA = 1;
    int testB = 1;
    int testC = 1;

    while (1)
    {
        while (gpio_get_pin_value(TEST_A))
        {
            gpio_set_pin_low(RESPONSE_A);
            testA = 0;
            while (!testA)
            {
                testA = gpio_get_pin_value(TEST_A);
            }
            gpio_set_pin_high(RESPONSE_A);
        }
        while (gpio_get_pin_value(TEST_B))
        {
            gpio_set_pin_low(RESPONSE_B);
            testB = 0;
            while (!testB)
            {
                testB = gpio_get_pin_value(TEST_B);
            }
            gpio_set_pin_high(RESPONSE_B);
        }
        while (gpio_get_pin_value(TEST_C))
        {
            gpio_set_pin_low(RESPONSE_C);
            testC = 0;
            while (!testC)
            {
                testC = gpio_get_pin_value(TEST_C);
            }
            gpio_set_pin_high(RESPONSE_C);
        }
    }
}