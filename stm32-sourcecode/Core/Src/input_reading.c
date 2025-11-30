/*
 * input_reading.c
 *
 *  Created on: Nov 20, 2025
 *      Author: nguye
 */
#include "main.h"
#include "input_reading.h"

// Debouncing
static GPIO_PinState buttonBuffer[N0_OF_BUTTONS];

//define two buffers for debouncing
static GPIO_PinState debounceButtonBuffer1[N0_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer2[N0_OF_BUTTONS];

//define a flag for a button pressed more than 1 second.
static uint8_t flagForButtonPress1s[N0_OF_BUTTONS];

//define counter for automatically increasing the value
//after the button is pressed more than 1 second.
static uint16_t counterForButtonPress1s[N0_OF_BUTTONS];


GPIO_PinState read_button(int index)
{
    switch (index)
    {
        case 0:
        	return HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin);
        case 1:
        	return HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin);
        case 2:
        	return HAL_GPIO_ReadPin(BTN3_GPIO_Port, BTN3_Pin);
        default:
        	return GPIO_PIN_SET;
    }
}


void button_reading(void) {
	for (uint8_t i = 0; i < N0_OF_BUTTONS; i++)
	{
		debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
		debounceButtonBuffer1[i] = read_button(i);
		if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i])
		{
			buttonBuffer[i] = debounceButtonBuffer1[i];
			if (buttonBuffer[i] == BUTTON_IS_PRESSED)
			{
				 //if a button is pressed, we start counting
				 if(counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING){
					 counterForButtonPress1s[i]++;
				 } else {
					 //the flag is turned on when 1 second has passed
					 //since the button is pressed.
					 flagForButtonPress1s[i] = 1;
					 //todo
				 }
			} else {
				 counterForButtonPress1s[i] = 0;
				 flagForButtonPress1s[i] = 0;
			}
		}
	}
}

unsigned int is_button_pressed(unsigned int index) {
	if(index >= N0_OF_BUTTONS) return 0;
	return (buttonBuffer[index] == BUTTON_IS_PRESSED);
}


unsigned int is_button_pressed_1s(unsigned int index){
	if(index >= N0_OF_BUTTONS) return 0;
	return (flagForButtonPress1s[index] == 1);
}




