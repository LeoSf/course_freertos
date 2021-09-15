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


In the new STM32CubeIDE the code structure is realy hihg level since in the main.c file you are going to find only calls to functions and macros. This is suitable since the MX tool manages the comple HAL. The only problem could be that in future versions the current code will no be compatible... (as always) but it really depends on the manufacturer.


#### UART MX Configuration

Mode: asynchronous
HW flow control: disable

baud rate: 209700

If this have to be done manually, in NUCLEO-L5 we have:
> LPUART1 (PG7/PG8) connected to ST-LINK VCP.

in the datasheet of the chip 
* Section: 4 Pinouts and pin description
    * Table 22. Alternate function AF0 to AF7 (continued)

So in the datasheet the alternative functions are divided in two tables:
* alternative functions from AF0 to AF7
* alternative functions from AF8 to AF15

So to find the PG7 you need to check both tables. You can find the uart function int the second one under funtion **AF8** on **page 136**. So:
* LPUART1_TX (Port G, pin 7): AF8 
* LPUART1_RX (Port G, pin 8): AF8 


### 4. UART pin configuration using STD. Periph. Libray APIs

Course example:
```c
static void pvrSetupHardware(void)
{
    GPIO_InitTypeDef gpio_uart_pins;
    USART_InitTypeDef uart2_init;

   GPIO InitTypedef gpio uart_pins;

    //1. Enable the UARTZ and GPIOA Peripheral clock
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); 
    RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIDA, ENABLE);

    //PA2 is UART2_TX, PA3 is AURT2_RX

    //zeroing each and every member element of the structure
    memset(&gpio_uart_pins,0, sizeof(gpio_uart_pins));

    //2. Alternate function configuration of MCU mins to beltage as UARTZ TX and RX
    gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
    gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
    
    GPIO_Init(GPIOA, &gpio_uart_mins);

    3. AF mode settings for the pins
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //PA2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //PA3

    //4. UART parameter initializations

    //zeroing each and every member element of the structure
    memset(&uart2_init,0, sizeof(uart2_init));

    uart2_init.USART_BaudRate = 115200;
    uart2_init.USART_HardwareFlowControl = USART_Hardware_FlowControl_None; uart2_init.USART_Mode = USART_Mode_Tx | USART_ Mode_Rx;
    uart2_init.USART_Parity = USART_Parity_No;
    uart2_init.USART_StopBits = USART_StopBits_1; 
    uart2_init.USART_WordLength = USART_WordLength_8b; 
    USART_Init(USART2, &uart2_init);
}
```

### 6. Exercise  UART Printmsg implementation using std periph. libary

Some interesting aproach to print msgs.



