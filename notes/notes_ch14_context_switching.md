# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch14. Context switching

### 14.1. Introduction to context switching

#### What is Context Switching?

* Context switching is a process of switching out of one task and switching in of another task on the CPU to execute.
* In RTOS, Context Switching is taken care by the Scheduler.
* In FreeRTOS Context Switching is taken care by the **PendSV Handler** found in **port.c**
* Whether context switch should happen or not depends upon the scheduling policy of the scheduler.


* If the scheduler is priority based pre-emptive scheduler, then for every RTOS tick interrupt, the scheduler will compare the priority of the running task with the priority of ready tasks list. If there is any ready task whose priority is higher than the running task then context switch will occur.
* On FreeRTOS you can also trigger context switch manually using **taskYIELD()** macro
* Context switch also happens immediately whenever new task unblocks and if its priority is higher than the currently running task.

in main:
```c
taskYIELD();
   
```

in task.h
```c
#define taskYIELD()					portYIELD()
```

in portmacro.h
```c
/* Scheduler utilities. */
#define portYIELD()					vPortYield()
```

in port.c
```c
void vPortYield( void ) /* PRIVILEGED_FUNCTION */
{
	/* Set a PendSV to request a context switch. */
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;

	/* Barriers are normally not required but do ensure the code is
	 * completely within the specified behaviour for the architecture. */
	__asm volatile( "dsb" ::: "memory" );
	__asm volatile( "isb" );
}
```

#### Task State

When a task executes on the Processor it utilizes

* Processor core registers.
*  If a Task wants to do any push and pop operations (during function call) then it uses its own dedicated stack memory.


```
State of the task = Contents of the processor core registers  + Stack contents
```

### 14.2. Context Switching  understanding State of a Task

#### ARM Cortex Mx Core Registers



### 14.3. Context Switching  Task Switching out procedure with animation

### 14.4. Context switching  understanding pendSV handler code

### 14.5. Understanding SystemView trace when preemption is off
