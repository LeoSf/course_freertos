# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch8. FreeRTOS Hello World App and Testing on hardware


* Case 1: Use ARM Semi-hosting feature to print logs on the console
* Case 2: Use UART peripheral of the MCU to print logs


Clock of the uC (derived from SYSCLCK):
* HSE: high speed external (external cristal)
* HSI: high speed internal (from the RC oscilator from inside the uC)
* PLL: clock generator engine. (to reach max frequency operation)

This information can be found in section **Reset and Clock Control (RCC)** of the datasheet.

### Reset handler

The first handler that is excecutd is the the reset handler. 

File: **startup_stm32l552zetxq.s**

```c
Reset_Handler:
  ldr   sp, =_estack    /* set stack pointer */

/* Call the clock system initialization function.*/
  bl  SystemInit

/* Copy the data segment initializers from flash to SRAM */
  movs	r1, #0
  b	LoopCopyDataInit
```

**SystemInit** is implemented in **system_stm32l5xx.c**.