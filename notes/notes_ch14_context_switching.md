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
```cpp
#define taskYIELD()     portYIELD()
```

in portmacro.h
```cpp
/* Scheduler utilities. */
#define portYIELD()     vPortYield()
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

![cortex_registers](img/cortex_registers.png)

Pointers for stack operation:
* **PSP**: process stack pointer (user tasks)
* **MSP**: main stack pointer (kernel tasks)

In FreeRTOS each user task will have it private stack, so tasks use PSP, whereas kernel uses MSP.

There are mainly two different Stack Memories during run time of FreeRTOS based application:
* Task's private stack (process stack)
* Kernel Stack (Main stack)

* when a **task** exceutes, it does PUSH and POP in the **Process Stack** using the **SP(PSP)**
* when a **ISR** exceutes, it does PUSH and POP in the **Main Stack** using the **SP(MSP)**


#### Stacks

![heap_kernel_stack](img/heap_kernel_stack.png)


Remember:

![task_creation](img/task_creation.png)


### 14.3. Context Switching: Task Switching OUT procedure with animation

#### Task switching out procedure

Before task is switched out, following things have to be taken care.

1. Processor core registers RO, R1, R2, R3, R12, LR, PC, xPSR(stack frame) are saved on to the task's private stack automatically by the processor **SysTick interrupt entry sequence**.

2. If Context Switch is required then SysTick timer will pend the PendSV Exception and PendSV handler runs

3. Processor core registers (R4-R11, R14) have to be saved manually on the task's private stack memory (**Saving the context**)

4. Save the new top of stack value (PSP) into first member of the TCB

5. Select the next potential Task to execute on the CPU. Taken care by **vTaskSwitchContext()** implemented in **tasks.c**

```c
/* Select a new task to run using either the generic C or port optimised asm code. */ 
taskSELECT_HIGHEST_PRIORITY_TASK();   
```

![context_out_1](img/context_out_1.png)
![context_out_2](img/context_out_2.png)
![context_out_3](img/context_out_3.png)
![context_out_4](img/context_out_4.png)
![context_out_5](img/context_out_5.png)


Some notes:
PSP: process stack pointer
    * for user task
MSP: main stack pointer
    * for kernel

stmdb store with  decrement before
stack will be decremented first and then value will be push in the stack whose address is reference by the addres of r0 (r0!)

str r0, [r2] is saving of PSP or the latest stack addr into TCB (of the task running?)

remember: the fist element of the TCB is the top of stack 

### 14.4. Context switching: understanding pendSV handler code (Task Switching In Procedure)

*Explanation of the assembly code of:*
```c
void PendSV_Handler( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
```

#### Task Switching In Procedure

So, at this time, we already know which task (TCB) should be switched in.That means new switchable task's TCB can be accessed by **pxCurrentTCB**.

1. First get the address of top of stack. Copy the value of **pxTopOfStack** in to **PSP register**.
2. Pop all the registers (R4-R11, R14) **(Restoring the context)**
3. Exception exit : Now PSP is pointing to the start address of the stack frame which will be popped out automatically due to exception exit.

![context_in_1](img/context_in_1.png)
![context_in_2](img/context_in_2.png)
![context_in_3](img/context_in_3.png)
![context_in_4](img/context_in_4.png)

### 14.5. Understanding SystemView trace when preemption is off

```c
/* segger info message for Segger SystemView */
SEGGER_SYSVIEW_PrintfTarget(msg);   
```

Description of the captured data for the following acquisition modes:
1. 003_freertos_poc_uart_w_info.SVdat: timers enabled, preemption enabled, but without tracing the vPortYield function.
2. 003_freertos_poc_uart_w_info2.SVdat: timers enabled, preemption enabled, and adding the trace for the vPortYield function.
3. 003_freertos_poc_uart_w_info3.SVdat: trace without vPortYield calls, so each task switch with a period of 1 ms.
4. 003_freertos_poc_uart_w_info4.SVdat: trace without timers enabled and with vPortYield.
5. 003_freertos_poc_uart_w_info5.SVdat: timers enabled, without preemption in colaborative mode
6. 003_freertos_poc_uart_w_info5b.SVdat: same as before but the trace is longer.

More notes:

I've tried to capture data to make run FreeRTOS in the collaborative mode in all these different attempts. Nevertheless, I had some problems with the config files, so I registered everything to have more information to analyze what was happening. Also, in the configured setup, I tried to use the UART to send a message of which task was active and a segger print statement to indicate when a task was executing the yield call.

Description of the captured data for the following acquisition modes:
1. 003_freertos_poc_uart_w_info.SVdat: timers enabled, preemption enabled, but without tracing the vPortYield function.
2. 003_freertos_poc_uart_w_info2.SVdat: timers enabled, preemption enabled, and adding the trace for the vPortYield function.
3. 003_freertos_poc_uart_w_info3.SVdat: trace without vPortYield calls, so each task switch with a period of 1 ms.
4. 003_freertos_poc_uart_w_info4.SVdat: preemption is enabled, timers enabled, vPortYield calls but without UART messages send calls. 
5. 003_freertos_poc_uart_w_info5.SVdat: timers enabled, without preemption in colaborative mode
6. 003_freertos_poc_uart_w_info5b.SVdat: same as before but the trace is longer.

In (1), you can see a transition from task to task without the timer call and without the scheduler, but this is not true since the scheduler is the one making the context switch.

Another important thing is that the segger print function is executed only when the timer interrupt doesn't take place to switch the context.

In (2), now we can see the scheduler task taking place after a vPortYield call from a task. Although, the same problem with the segger print is happening.

In (3), we don't have a segger print statement probably because the timer is switching the context before that...

In (4), timers are enabled because, after the systick, there is a scheduler task call, but I suppose that the graph corresponds to a program where there is no UART call, so the task duration is shorter, and there are multiple vPortYield. But, after a period of 1 ms there is a timer interrupt... it is a little bit messy.

In (5), everything is working as expected, but the tasks are always active; because of that, the background color is darker. It's not like in the preemptive mode where after the context switch, tasks were not active. 

Everything is the same in (5b), but the trace is longer than in (5) since a proper segger configuration was applied.