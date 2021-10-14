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

