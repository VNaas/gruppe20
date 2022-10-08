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
    int pinB = gpio_get_pin_interrupt_flag(TEST_B);
    int pinC = gpio_get_pin_interrupt_flag(TEST_C);
    if (pinA)
    {
        gpio_set_pin_low(RESPONSE_A);
        int test_A = 0;
        while (!test_A)
        {
            test_A = gpio_get_pin_value(TEST_A);
        }
        gpio_set_pin_high(RESPONSE_A);
        gpio_clear_pin_interrupt_flag(TEST_A)
    }
    if (pinB)
    {
        gpio_set_pin_low(RESPONSE_B);
        int test_B = 0;
        while (!test_B)
        {
            test_B = gpio_get_pin_value(TEST_B);
        }
        gpio_set_pin_high(RESPONSE_B);
        gpio_clear_pin_interrupt_flag(TEST_B)
    }
    if (pinC)
    {
        gpio_set_pin_low(RESPONSE_C);
        int test_C = 0;
        while (!test_C)
        {
            test_C = gpio_get_pin_value(TEST_C);
        }
        gpio_set_pin_high(RESPONSE_C);
        gpio_clear_pin_interrupt_flag(TEST_C)
    }
}

int main(void)
{
    init();

    int interrupts = 0;
    int polling = 0;
    int all = 0;
    if (polling)
    {
        if (all)
        {
            testAll();
        }
        else
        {
            testA();
        }
    }
    else if (interrupts)
    {
        gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
        gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
        gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
    }

    while (1)
    {
        gpio_toggle_pin(LED0_GPIO);

        printf("tick\n");

        busy_delay_ms(500);
        // testA();
        // testAll();
    }
}
