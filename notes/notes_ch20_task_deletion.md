# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch20. FreeRTOS Task Deletion

### 20.1. Deleting a Task

```c
/**
 * task. h
 * <pre>void vTaskDelete( TaskHandle_t xTask );</pre>
 *
 * INCLUDE_vTaskDelete must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Remove a task from the RTOS real time kernel's management.  The task being
 * deleted will be removed from all ready, blocked, suspended and event lists.
 *
 * NOTE:  The idle task is responsible for freeing the kernel allocated
 * memory from tasks that have been deleted.  It is therefore important that
 * the idle task is not starved of microcontroller processing time if your
 * application makes any calls to vTaskDelete ().  Memory allocated by the
 * task code is not automatically freed, and should be freed before the task
 * is deleted.
 *
 * See the demo application file death.c for sample code that utilises
 * vTaskDelete ().
 *
 * @param xTask The handle of the task to be deleted.  Passing NULL will
 * cause the calling task to be deleted.
 *
 * Example usage:
   <pre>
 void vOtherFunction( void )
 {
 TaskHandle_t xHandle;

	 // Create the task, storing the handle.
	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );

	 // Use the handle to delete the task.
	 vTaskDelete( xHandle );
 }
   </pre>
 * \defgroup vTaskDelete vTaskDelete
 * \ingroup Tasks
 */
void vTaskDelete( TaskHandle_t xTaskToDelete ) PRIVILEGED_FUNCTION;
```

Remember that when you use vTaskDelete() it won't deallocate the memory occupied by the task. It just marks the Task as deleted. Later when the idle task runs it will de-allocate the memory for all the tasks which are marked as deleted.


**In real-time programming, you will see hardly any scenario where you want to delete a task.**

### 20.2. Exercise  Task Delete


Write an application which launches 2 tasks task1 and task2.

* task1 priority = 1
* task2 priority = 2

task 2 should toggle the LED for every 1 sec and should delete itself when button is pressed by the user.

task1 should toggle the same led for every 200ms.


### 20.3. Exercise  Task Delete Implementation