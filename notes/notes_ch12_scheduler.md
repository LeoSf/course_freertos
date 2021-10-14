# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch12. FreeRTOS Scheduler

### 12.1. Overview of FreeRTOS Scheduler


The Scheduler decides which thread should be executing by examining the priority assigned to each thread by the application writer.

In the simplest design higher priority can be assigned to those threads that implement hard real requirements and lower priorities to threads that implement soft real time requirements.

#### Why do we need Scheduler ?

1. It just a piece of code which implements task switching in and Task switching out according to the scheduling policy selected.
2. Scheduler is the reason why multiple tasks run on your system efficiently
3. The basic job of the scheduler is to determine which is the next potential task to run on the CPU
4. Scheduler has the ability to preempt a running task if you configure so.


#### Scheduling Policies (Scheduler types) 


1. simple Pre-emptive Scheduling (Round robin)
2. Priority based Pre-Emptive Scheduling
4. Co-operative Scheduling

The **scheduling policy** is the algorithm used by the scheduler to decide which task to execute at any point in time.

FreeRTOS or most of the Real Time OS most likely would be using Priority based **Pre-emptive Scheduling** by default.

### 12.2. FreeRTOS Scheduler and xTaskStartScheduler() API explanation

#### FreeRTOS Scheduling

* Scheduler is a piece of kernel code responsible for deciding which task should be executing at any particular time on the CPU.
* The scheduling policy is the algorithm used by the scheduler to decide which task to execute at any point in time.
* **configUSE_PREEMPTION** of freeRTOSConfig.h configurable item decides the scheduling policy in freeRTOS.
* If **configUSE_PREEMPTION = 1**, then scheduling policy will be priority based pre emptive scheduling.
* If **configUSE_PREEMPTION = 0**, then scheduling policy will be cooperative scheduling

#### FreeRTOS Scheduler Implementation

In FreeRTOS the scheduler code is actually combination of **FreeRTOS Generic code + Architecture specific codes**

* Scheduler:
   * FreeRTOS Generic Code found in tasks.c
   * Architecture Specific codes of port.c

#### Architecture specific codes responsible to achieve scheduling of tasks.

All architecture specific codes and configurations are implemented in:
* port.c and 
* portmacro.h

If you are using ARM Cortex Mx processor then you should be able locate the below interrupt handlers in **port.c** which are part of the scheduler implementation of freeRTOS.

Three important kernel interrupt handlers responsible for scheduling of tasks

* vPortSVCHandler()
   * Used to launch the very first task. 
   * Triggered by SVC instruction
* xPortPendSVHandler()
   * Used to achieve the context switching between tasks 
   * Triggered by pending the PendSV System exception of ARM
* xPortSysTickHandler()
   * This implements the RTOS Tick management. 
   * Triggered periodically by Systick timer of ARM Cortex Mx processor

**SV stand for service or supervisor**. Examples:
* PendSV (Pendable SerVice) is an interrupt request is used by the OS to force a context switch if no other interrupt is active.
* SVCall or SVC (SuperVisor Call) is triggered by the SVC instruction and is used by the FreeRTOS to start the scheduler. 


### 12.3. Understanding implementation of xPortStartScheduler() of port.c

#### vTaskStartScheduler()

* This is implemented in tasks.c of FreeRTOS kernel and used to start the RTOS scheduler.
* Remember that after calling this function only the scheduler code is initialized and all the Arch. Specific interrupts will be activated.
* This function also creates the idle and Timer daemon task
* This function calls xPortStartScheduler() to do the Arch. Specific Initializations


Funtion calls:

* **vTaskStartScheduler()**: Generic Code for All Arch.

   * **xPortStartScheduler()**: Arch. Specific code
      * Configuring the SysTick timer to issue interrupts at desired rate (as configured in the config item config **TICK_RATE_HZ** in **FreeRTOSConfig.h**)
      * Configures the priority for PendSV and Systick interrupts.
      * Starts the first task by executing the SVC instruction

#### TODO full description of the xPortPendSVHandler


```asm
void PendSV_Handler( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile                                      
    (                                                   
    "    .syntax unified                                \n"
    "                                                   \n"
    "    mrs r0, psp                                    \n" /* Read PSP in r0. */
    #if( configENABLE_FPU == 1 )
    "    tst lr, #0x10                                  \n" /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
    "    it eq                                          \n"
    "    vstmdbeq r0!, {s16-s31}                        \n" /* Store the FPU registers which are not saved automatically. */
    #endif /* configENABLE_FPU */
    #if( configENABLE_MPU == 1 )
    "    mrs r1, psplim                                 \n" /* r1 = PSPLIM. */
    "    mrs r2, control                                \n" /* r2 = CONTROL. */
    "    mov r3, lr                                     \n" /* r3 = LR/EXC_RETURN. */
    "    stmdb r0!, {r1-r11}                            \n" /* Store on the stack - PSPLIM, CONTROL, LR and registers that are not automatically saved. */
    #else /* configENABLE_MPU */
    "    mrs r2, psplim                                 \n" /* r2 = PSPLIM. */
    "    mov r3, lr                                     \n" /* r3 = LR/EXC_RETURN. */
    "    stmdb r0!, {r2-r11}                            \n" /* Store on the stack - PSPLIM, LR and registers that are not automatically saved. */
    #endif /* configENABLE_MPU */
    "                                                   \n"
    "    ldr r2, pxCurrentTCBConst                      \n" /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    "    ldr r1, [r2]                                   \n" /* Read pxCurrentTCB. */
    "    str r0, [r1]                                   \n" /* Save the new top of stack in TCB. */
    "                                                   \n"
    "    mov r0, %0                                     \n" /* r0 = configMAX_SYSCALL_INTERRUPT_PRIORITY */
    "    msr basepri, r0                                \n" /* Disable interrupts upto configMAX_SYSCALL_INTERRUPT_PRIORITY. */
    "    dsb                                            \n"
    "    isb                                            \n"
    "    bl vTaskSwitchContext                          \n"
    "    mov r0, #0                                     \n" /* r0 = 0. */
    "    msr basepri, r0                                \n" /* Enable interrupts. */
    "                                                   \n"
    "    ldr r2, pxCurrentTCBConst                      \n" /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    "    ldr r1, [r2]                                   \n" /* Read pxCurrentTCB. */
    "    ldr r0, [r1]                                   \n" /* The first item in pxCurrentTCB is the task top of stack. r0 now points to the top of stack. */
    "                                                   \n"
    #if( configENABLE_MPU == 1 )
    "    dmb                                            \n" /* Complete outstanding transfers before disabling MPU. */
    "    ldr r2, xMPUCTRLConst                          \n" /* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
    "    ldr r4, [r2]                                   \n" /* Read the value of MPU_CTRL. */
    "    bic r4, #1                                     \n" /* r4 = r4 & ~1 i.e. Clear the bit 0 in r4. */
    "    str r4, [r2]                                   \n" /* Disable MPU. */
    "                                                   \n"
    "    adds r1, #4                                    \n" /* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
    "    ldr r3, [r1]                                   \n" /* r3 = *r1 i.e. r3 = MAIR0. */
    "    ldr r2, xMAIR0Const                            \n" /* r2 = 0xe000edc0 [Location of MAIR0]. */
    "    str r3, [r2]                                   \n" /* Program MAIR0. */
    "    ldr r2, xRNRConst                              \n" /* r2 = 0xe000ed98 [Location of RNR]. */
    "    movs r3, #4                                    \n" /* r3 = 4. */
    "    str r3, [r2]                                   \n" /* Program RNR = 4. */
    "    adds r1, #4                                    \n" /* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
    "    ldr r2, xRBARConst                             \n" /* r2 = 0xe000ed9c [Location of RBAR]. */
    "    ldmia r1!, {r4-r11}                            \n" /* Read 4 sets of RBAR/RLAR registers from TCB. */
    "    stmia r2!, {r4-r11}                            \n" /* Write 4 set of RBAR/RLAR registers using alias registers. */
    "                                                   \n"
    "    ldr r2, xMPUCTRLConst                          \n" /* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
    "    ldr r4, [r2]                                   \n" /* Read the value of MPU_CTRL. */
    "    orr r4, #1                                     \n" /* r4 = r4 | 1 i.e. Set the bit 0 in r4. */
    "    str r4, [r2]                                   \n" /* Enable MPU. */
    "    dsb                                            \n" /* Force memory writes before continuing. */
    #endif /* configENABLE_MPU */
    "                                                   \n"
    #if( configENABLE_MPU == 1 )
    "    ldmia r0!, {r1-r11}                            \n" /* Read from stack - r1 = PSPLIM, r2 = CONTROL, r3 = LR and r4-r11 restored. */
    #else /* configENABLE_MPU */
    "    ldmia r0!, {r2-r11}                            \n" /* Read from stack - r2 = PSPLIM, r3 = LR and r4-r11 restored. */
    #endif /* configENABLE_MPU */
    "                                                   \n"
    #if( configENABLE_FPU == 1 )
    "    tst r3, #0x10                                  \n" /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
    "    it eq                                          \n"
    "    vldmiaeq r0!, {s16-s31}                        \n" /* Restore the FPU registers which are not restored automatically. */
    #endif /* configENABLE_FPU */
    "                                                   \n"
    #if( configENABLE_MPU == 1 )
    "    msr psplim, r1                                 \n" /* Restore the PSPLIM register value for the task. */
    "    msr control, r2                                \n" /* Restore the CONTROL register value for the task. */
    #else /* configENABLE_MPU */
    "    msr psplim, r2                                 \n" /* Restore the PSPLIM register value for the task. */
    #endif /* configENABLE_MPU */
    "    msr psp, r0                                    \n" /* Remember the new top of stack for the task. */
    "    bx r3                                          \n"
    "                                                   \n"
    "    .align 4                                       \n"
    "pxCurrentTCBConst: .word pxCurrentTCB              \n"
    #if( configENABLE_MPU == 1 )
    "xMPUCTRLConst: .word 0xe000ed94                    \n"
    "xMAIR0Const: .word 0xe000edc0                      \n"
    "xRNRConst: .word 0xe000ed98                        \n"
    "xRBARConst: .word 0xe000ed9c                       \n"
    #endif /* configENABLE_MPU */
    :: "i"( configMAX_SYSCALL_INTERRUPT_PRIORITY )
    );
}
```