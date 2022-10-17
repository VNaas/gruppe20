#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gpio.h>
#include <power_clocks_lib.h>
#include <sysclk.h>
#include <compiler.h>
#include <board.h>
#include <stdio_usb.h>


#define CONFIG_USART_IF (AVR32_USART2)

#include "FreeRTOS.h"
#include "task.h"
#define INCLUDE vTaskDelay 1 

#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


void busy_delay_ms(int delay){
    for(; delay != 0; delay--){
        for(int i = 0; i < 2108; i++){
            asm volatile ("" ::: "memory");
        }
    }
}

void busy_delay_short(void){
    for(int i = 0; i < 10; i++){
        asm volatile ("" ::: "memory");
    }
}

void init(){
	board_init();
	
    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_B, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_C, GPIO_DIR_INPUT);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
	
	stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
	    setbuf(stdout, NULL);
	    setbuf(stdin,  NULL);
    #endif
}

static void taskFn(void* args){
	const portTickType delay = 1000 / portTICK_RATE_MS;
	
    int iter = 0;

	while(1){
		gpio_toggle_pin(LED0_GPIO);
		printf("tick %d\n", iter++);
		
		vTaskDelay(delay);
	}
}

static void taskA_1(void* args{
	const portTickType delay = 200 / portTICK_RATE_MS;
    int iter = 0;

    while (1)
    {
        gpio_toggle_pin(LED0_GPIO)
        printf("tick LED0 %d\n", iter++);
        vTaskDelay(delay);
    }
}
static void taskA_2(void* args){
	const portTickType delay = 500 / portTICK_RATE_MS;
    int iter = 0;
    while (1)
    {
        gpio_toggle_pin(LED0_GPI1)
        printf("tick LED1 %d\n", iter++);    
        vTaskDelay(delay);
    }
}
//TASK B
struct responseTaskArgs {
    struct { 
        uint32_t test; 
        uint32_t response;
    } pin;
// other args ...
};

static void responseTask(void* args){
    struct responseTaskArgs a = *(struct responseTaskArgs*)args;
    while(1){
        if(gpio_pin_is_low(a.pin.test)){

        }
    }
}

static void responseTaskB(void* args){
    //The tasks should detect a test signal from the BRTT by continuously reading the value (busy-wait). 
    //When the signal is detected, the task should send the response signal by setting the response pin low.
    while(1){
		if(gpio_pin_is_low(a.pin.test)){
			gpio_set_pin_low(a.pin.response);
            vTaskDelay(0) //Skal den her eller etter begge pin interaction?
			gpio_set_pin_high(a.pin.response);
		}
	}
}

static void responseTaskC(void* args){
    // Again, the ”work” is just a call to busy wait ms().  
    //When the signal is detected, the task should send the response signal by setting the response pin low.
    //Make task C wait 3ms before responding, while tasks A and B respond immediately.
    while(1){
		if(gpio_pin_is_low(a.pin.test)){
            if (a.pin.test == TEST_C){
				busy_delay_ms(3);
			}
			gpio_set_pin_low(a.pin.response);
            vTaskDelay(0) //Skal den her eller etter begge pin interaction?
			gpio_set_pin_high(a.pin.response);
	}
}

static void responseTaskC(void* args){
    // Instead of busy-waiting for the test pin to go low, we want to check it intermittently. 
    //If the pin is not low, we can wait for the next scheduler tick by calling vTaskDelay(1), 
    //then loop around and test the pin again once the scheduler wakes us. 

    //DEnne er jeg usikker på. 
    while(1){
		if(gpio_pin_is_low(a.pin.test)){
			if (a.pin.test == TEST_C){
				busy_delay_ms(3);
			}
			
			gpio_set_pin_low(a.pin.response);
			vTaskDelay(1);
			gpio_set_pin_high(a.pin.response);
		} else {
			vTaskDelay(1);
		}
	}
}



int main(){
	init();
    
  
    //TASK A:
	xTaskCreate(taskA_1, "", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(taskA_2, "", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
    
    //TASK B:
	xTaskCreate(responseTaskB, "", 1024,(&(struct responseTaskArgs){{TEST_A, RESPONSE_A}}),tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(responseTaskB, "", 1024,(&(struct responseTaskArgs){{TEST_B, RESPONSE_B}}),tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(responseTaskB, "", 1024,(&(struct responseTaskArgs){{TEST_C, RESPONSE_C}}),tskIDLE_PRIORITY + 1, NULL);

    //TASK C:
    xTaskCreate(responseTaskC, "", 1024,(&(struct responseTaskArgs){{TEST_A, RESPONSE_A}}),tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(responseTaskC, "", 1024,(&(struct responseTaskArgs){{TEST_B, RESPONSE_B}}),tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(responseTaskC, "", 1024,(&(struct responseTaskArgs){{TEST_C, RESPONSE_C}}),tskIDLE_PRIORITY + 1, NULL);

    //TASK D:
    xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}}), tskIDLE_PRIORITY + 1, NULL);
    
    
    // Start the scheduler, anything after this will not run.
	vTaskStartScheduler();
    
}

