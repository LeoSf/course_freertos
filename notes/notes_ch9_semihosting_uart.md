# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch9. Semi hosting and UART setup

![semihosting_overview](img/semihosting_overview.png)


### Using semihosting 


1. Linker argument settings
> C and C++ build > linker > misc 
> -specs=rdimon.specs -lc -lrdimon

2. Debug configuration of your application 
Debug as > debug configuration > select the project > tab: Startup > Run command: 
> monitor arm semihosting enable

3. in main.c use below codes
```c
extern void initialise_monitor_handles();
initialise_monitor_handles();

```

### Using the UART



