#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"
#include "BRTT.h"

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A AVR32_PIN_PA31
#define RESPONSE_A AVR32_PIN_PA30
#define TEST_B AVR32_PIN_PA29
#define RESPONSE_B AVR32_PIN_PA28
#define TEST_C AVR32_PIN_PA27
#define RESPONSE_C AVR32_PIN_PB00

__attribute__((__interrupt__)) static void interrupt_J3(void);

void init()
{
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);

    cpu_irq_disable();
    INTC_init_interrupts();
    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
    cpu_irq_enable();

    stdio_usb_init(&CONFIG_USART_IF);

#if defined(__GNUC__) && defined(__AVR32__)
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
#endif
}

__attribute__((__interrupt__)) static void interrupt_J3(void)
{
    int pinA = gpio_get_pin_interrupt_flag(TEST_A);
    if (pinA)
    {
        // Probably more efficient to respond here
        testAFlag = 1;
        gpio_clear_pin_interrupt_flag(TEST_A);
    }
    int pinB = gpio_get_pin_interrupt_flag(TEST_B);
    if (pinB)
    {
        testBFlag = 1;
    }
    int pinC = gpio_get_pin_interrupt_flag(TEST_C);
    if (pinC)
    {
        testCFlag = 1;
        gpio_clear_pin_interrupt_flag(TEST_C);
    }
}

volatile int testAFlag = 0;
volatile int testAIntrFlag = 0;
volatile int testBFlag = 0;
volatile int testBIntrFlag = 0;
volatile int testCFlag = 0;
volatile int testCIntrFlag = 0;
int main(void)
{
    init();

    gpio_enable_pin_interrupt(TEST_A, GPIO_BOTHEDGES); // GPIO_PIN_CHANGE?
    gpio_enable_pin_interrupt(TEST_B, GPIO_BOTHEDGES);
    gpio_enable_pin_interrupt(TEST_C, GPIO_BOTHEDGES);

    while (1)
    {
        if (testAFlag)
        {
            gpio_set_pin_low(RESPONSE_A);
            if (gpio_get_pin_value(TEST_A))
            {
                gpio_set_pin_high(RESPONSE_A);
            }
            testAFlag = 0;
        }
        if (testBFlag)
        {
            gpio_set_pin_low(RESPONSE_B);
            if (gpio_get_pin_value(TEST_B))
            {
                gpio_set_pin_high(RESPONSE_B);
            }
            testBFlag = 0;
        }
        if (testCFlag)
        {
            gpio_set_pin_low(RESPONSE_C);
            if (gpio_get_pin_value(TEST_C))
            {
                gpio_set_pin_high(RESPONSE_C);
            }
            testCFlag = 0;
        }
    }
}
