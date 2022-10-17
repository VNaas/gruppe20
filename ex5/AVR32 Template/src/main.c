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
    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(TEST_B, GPIO_DIR_INPUT);
    gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(TEST_C, GPIO_DIR_INPUT);
    gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	cpu_irq_disable();
	gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE); // GPIO_PIN_CHANGE?
	gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
	INTC_init_interrupts();
	INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
	cpu_irq_enable();

	stdio_usb_init(&CONFIG_USART_IF);

	#if defined(__GNUC__) && defined(__AVR32__)
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	#endif
}

volatile int testAFlag = 0;
volatile int testBFlag = 0;
volatile int testCFlag = 0;

__attribute__((__interrupt__)) static void interrupt_J3(void)
{
    // gpio_set_pin_high(LED0_GPIO);
    
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
		busy_delay_us(100);
		testBFlag = 1;
		gpio_clear_pin_interrupt_flag(TEST_B);
	}
	int pinC = gpio_get_pin_interrupt_flag(TEST_C);
	if (pinC)
	{
		testCFlag = 1;
		gpio_clear_pin_interrupt_flag(TEST_C);
	}
}


int main(void)
{
	init();
	printf("Hei\n");

	while (1)
	{
        //gpio_toggle_pin(LED0_GPIO);

        //printf("tick\n");

        //busy_delay_ms(500);
		if (testAFlag)
		{
			gpio_set_pin_low(RESPONSE_A);
			int test_A = 0;
			while (!test_A)
			{
				test_A = gpio_get_pin_value(TEST_A);
			}
			gpio_set_pin_high(RESPONSE_A);
			testAFlag = 0;
		}
		if (testBFlag)
		{
			gpio_set_pin_low(RESPONSE_B);
			int test_B = 0;
			while (!test_B)
			{
				test_B = gpio_get_pin_value(TEST_B);
			}
			gpio_set_pin_high(RESPONSE_B);
			testBFlag = 0;
		}
		if (testCFlag)
		{
			gpio_set_pin_low(RESPONSE_C);
			int test_C = 0;
			while (!test_C)
			{
				test_C = gpio_get_pin_value(TEST_C);
			}
			gpio_set_pin_high(RESPONSE_C);
			testCFlag = 0;
		}
		
	}
}
