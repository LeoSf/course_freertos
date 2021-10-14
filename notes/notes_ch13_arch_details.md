# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch13. FreeRTOS and ARM Cortex Mx Arch. Specific details

### 13.1. FreeRTOS Kernel interrupts


When FreeRTOS runs on ARM Cortex Mx Processor based MCU, the below interrupts are used to implement the Scheduling of Tasks.

1. **SVC Interrupt** (SVC handler will be used to launch the very first Task )

2. **PendSV Interrupt** (PendSV handler is used to carry out context switching between tasks)

3. **SysTick Interrupt** (SysTick Handler implements the RTOS Tick Management)

* If SysTick interrupt is used for some other purpose in your application, then you may use any other available timer peripheral
* All interrupts are configured at the lowest interrupt priority possible.


### 13.2. RTOS Tick and SysTick Timer explanation

**RTOS Ticking is implemented using timer hardware of the MCU**

* Architecture ARM CORTEX M
    * Atmel SAMxx AT91SAM3X8E
    * STM ST32Fxx
    * LPC LPCxx
    * another manufacture family

> All of them call **SysTick Timer**

#### The RTOS Tick- Why it is needed?

* The simple answer is to keep track of time elapsed
* There is a global variable called "**xTickCount**", and it is incremented by one whenever tick interrupt occurs
* RTOS Ticking is implemented using SysTick timer of the ARM Cortex Mx processor.
* Tick interrupt happens at the rate of **configTICK_RATE_HZ** configured in the **FreeRTOSConfig.h**

#### The RTOS Tick- 

**Used for Context Switching to the next potential Task**

* Each timer Tick interrupt makes scheduler to run

1. The tick ISR runs
2. All the ready state tasks are scanned
3. Determines which is the next potential task to run
4. If found, triggers the context switching by pending the PendSV interrupt
5. The PendSV handler takes care of switching out of old task and switching in of new task

> Pending an interrupt also means "Activating" that interrupt.
> PendingSV is a system exception which can be triggered by enabling its pending bit in the corresponding register of the ARM Cortex Mx processor. 
Example: 
```c
   portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
```



### 13.3. Who configures the RTOS Tick Timer 

### 13.4. What RTOS Tick ISR (SysTick ISR) does  summary