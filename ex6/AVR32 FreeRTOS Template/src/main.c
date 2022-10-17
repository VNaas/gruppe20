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
	
	//stdio_usb_init(&CONFIG_USART_IF);
//
    //#if defined(__GNUC__) && defined(__AVR32__)
	    //setbuf(stdout, NULL);
	    //setbuf(stdin,  NULL);
    //#endif
}

static void taskFn(void* args){
	const portTickType delay = 1000 / portTICK_RATE_MS;
	
    int iter = 0;

	while(1){
		gpio_toggle_pin(LED0_GPIO);
		//printf("tick %d\n", iter++);
		
		vTaskDelay(delay);
	}
}

static void taskA_1(void* args){
	const portTickType delay = 200 / portTICK_RATE_MS;
    int iter = 0;

    while (1)
    {
        gpio_toggle_pin(LED0_GPIO);
        //printf("tick LED0 %d\n", iter++);
        vTaskDelay(delay);
    }
}
static void taskA_2(void* args){
	const portTickType delay = 500 / portTICK_RATE_MS;
    int iter = 0;
    while (1)
    {
        gpio_toggle_pin(LED1_GPIO);
        //printf("tick LED1 %d\n", iter++);    
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
	uint32_t delayDuration;
};


static void responseTaskB(void* args){
    //The tasks should detect a test signal from the BRTT by continuously reading the value (busy-wait). 
    //When the signal is detected, the task should send the response signal by setting the response pin low.
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
    while(1){
		busy_delay_short();
		if(gpio_pin_is_low(a.pin.test)){
			gpio_set_pin_low(a.pin.response);
            //Skal den her eller etter begge pin interaction?
			int pin_value = 0;
			while(!pin_value){
				pin_value = gpio_get_pin_value(a.pin.test);
			}
			vTaskDelay(a.delayDuration);
			gpio_set_pin_high(a.pin.response);
		}
	}
}

static void responseTaskC(void* args){
    // Again, the ”work” is just a call to busy wait ms().  
    //When the signal is detected, the task should send the response signal by setting the response pin low.
    //Make task C wait 3ms before responding, while tasks A and B respond immediately.
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
    while(1){
		if(gpio_pin_is_low(a.pin.test)){
            busy_delay_ms(a.delayDuration);
			gpio_set_pin_low(a.pin.response);
			
			int pin_value = 0;
			while(!pin_value){
				pin_value = gpio_get_pin_value(a.pin.test);
			}
            vTaskDelay(0);
			gpio_set_pin_high(a.pin.response);
		}
	}
}

static void responseTaskD(void* args){
    // Instead of busy-waiting for the test pin to go low, we want to check it intermittently. 
    //If the pin is not low, we can wait for the next scheduler tick by calling vTaskDelay(1), 
    //then loop around and test the pin again once the scheduler wakes us. 
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
    while(1){
		if(gpio_pin_is_low(a.pin.test)){
			busy_delay_ms(a.delayDuration);
			gpio_set_pin_low(a.pin.response);
			int pin_value = 0;
			while(!pin_value){
				pin_value = gpio_get_pin_value(a.pin.test);
			}
			vTaskDelay(0);
			gpio_set_pin_high(a.pin.response);
		} else {
			vTaskDelay(1);
		}
	}
}



int main(){
	init();
    
  
    //TASK A:
	//xTaskCreate(taskA_1, "", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(taskA_2, "", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
    
    //TASK B:
	//xTaskCreate(responseTaskB, "", 1024,(&(struct responseTaskArgs){{TEST_A, RESPONSE_A},0}),tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(responseTaskB, "", 1024,(&(struct responseTaskArgs){{TEST_B, RESPONSE_B},0}),tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(responseTaskB, "", 1024,(&(struct responseTaskArgs){{TEST_C, RESPONSE_C},0}),tskIDLE_PRIORITY + 1, NULL);

    ////TASK C:
    //xTaskCreate(responseTaskC, "", 1024,(&(struct responseTaskArgs){{TEST_A, RESPONSE_A},0}),tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(responseTaskC, "", 1024,(&(struct responseTaskArgs){{TEST_B, RESPONSE_B},0}),tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(responseTaskC, "", 1024,(&(struct responseTaskArgs){{TEST_C, RESPONSE_C},3}),tskIDLE_PRIORITY + 1, NULL);
//
    //TASK D:
    //xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A},0}), tskIDLE_PRIORITY + 1, NULL);
	//xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B},0}), tskIDLE_PRIORITY + 1, NULL);
	//xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C},3}), tskIDLE_PRIORITY + 1, NULL);
    
    //TASK E:
    xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A},0}), tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B},0}), tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(responseTaskD, "", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C},3}), tskIDLE_PRIORITY + 3, NULL);
// Start the scheduler, anything after this will not run.
	vTaskStartScheduler();
	
}

