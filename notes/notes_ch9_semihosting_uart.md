# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch9. Semi hosting and UART setup

![semihosting_overview](img/semihosting_overview.png)


### 8.2 Using semihosting 


1. Linker argument settings
> C and C++ build > linker > misc 
> -specs=rdimon.specs -lc -lrdimon

2. Debug configuration of your application 
Debug as > debug configuration > select the project > tab: Startup > Run command: 
> monitor arm semihosting enable

3. in main.c use below codes
```c
extern void initialise_monitor_handles();
```

4. in main function
```c
#ifdef USE_SEMIHOSTING
    initialise_monitor_handle();
    printf("Message in debug mode with semihosting");
#endif
``` 

### 8.3 Using the UART

Here 

Here is important to understand which UART is configured by default. From the UM2581 - User manual - STM32L5 Nucleo-144 board (MB1361) - section 6.1

![stm32_l5_uart_vcp](img/stm32_l5_uart_vcp.png)


From the figure the important thing is:

> LPUART1 (PG7/PG8) connected to ST-LINK VCP.  
Must be the interface for 1V8 MCU mode because PG [2-15] stay at 3V3 IO interface link to the VDDIO power supply.

So:
* PG7
    * type: LPUART1_TX
    * default name: ST-LINK_VCP_TX
* PG8: 
    * type: LPUART1_RX
    * default name: ST-LINK_VCP_RX


#### UART MX Configuration

Mode: asynchronous
HW flow control: disable

baud rate: 209700

