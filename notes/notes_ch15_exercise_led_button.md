# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch15. Exercise  Button and LED Task and Interrupt Coding

### 15.1. Exercise  Button and LED task

#### Exercise 001

Create 2 Tasks in your FreeRTOS application led_task and button_task.

Button Task should continuously poll the button status of the board and if pressed it should update the flag variable.

Led Task should turn on the LED if button flag is SET, otherwise it should turn off the LED. Use same freeRTOS task priorities for both the tasks.


#### nucleo-F4
On nucleo-F446RE board the LED is connected to PA5 pin and button is connected to PC13


### 15.7. Exercise  working with task and interrupt

#### Exercise 002

Write a FreeRTOS application which creates only 1 task : led_task and it should toggle the led when you press the button by checking the button status flag.

The button interrupt handler must update the button status flag.


### 15.8. Understanding GPIO Interrupts delivery in STM32 MCUs

### 15.9. Exercise  LED task and Button interrupt handling code implementation

### 15.10. Understanding Trace of button interrupt and led task execution