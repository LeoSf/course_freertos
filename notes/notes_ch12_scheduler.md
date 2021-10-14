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


### 12.3. Understanding implementation of xPortStartScheduler() of port.c

