# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch16. FreeRTOS Task Notification

### 16.1. Task Notify Introduction with exercise

#### Exercise

Write a program which creates 2 tasks **task_led** and **task_button** with equal priorities.

When button is pressed, task_button should notify the task_led and task_led should run on the CPU to toggle the LED. Also task_led should print how many times user has pressed the button so far.

**task_led** should not unnecessarily run on the CPU and it should be in Block mode until it receives the notification from the task_button.

### 16.2. Task notification APIs and xTaskNotifyWait()

RTOS Task Notification

Each RTOS task has a 32-bit notification value which is initialised to zero when the RTOS task is created.

An RTOS task notification is an event sent directly to a task that can unblock the receiving task, and optionally update the receiving task's notification value in a number of different ways. For example, a notification may overwrite the receiving task's notification value, or set one or more bits in the receiving task's notification value.

#### Wait and Notify APls

* xTaskNotifyWait()
* x TaskNotify()

#### xTaskNotifyWait()

If a task calls **xTaskNotifyWait()**, then it waits with an optional timeout until it receives a notification from some other task or interrupt handler.

task.c
```c
/* @param ulBitsToClearOnEntry Bits that are set in ulBitsToClearOnEntry value
 * will be cleared in the calling task's notification value before the task
 * checks to see if any notifications are pending, and optionally blocks if no
 * notifications are pending.  Setting ulBitsToClearOnEntry to ULONG_MAX (if
 * limits.h is included) or 0xffffffffUL (if limits.h is not included) will have
 * the effect of resetting the task's notification value to 0.  Setting
 * ulBitsToClearOnEntry to 0 will leave the task's notification value unchanged.
 *
 * @param ulBitsToClearOnExit If a notification is pending or received before
 * the calling task exits the xTaskNotifyWait() function then the task's
 * notification value (see the xTaskNotify() API function) is passed out using
 * the pulNotificationValue parameter.  Then any bits that are set in
 * ulBitsToClearOnExit will be cleared in the task's notification value (note
 * *pulNotificationValue is set before any bits are cleared).  Setting
 * ulBitsToClearOnExit to ULONG_MAX (if limits.h is included) or 0xffffffffUL
 * (if limits.h is not included) will have the effect of resetting the task's
 * notification value to 0 before the function exits.  Setting
 * ulBitsToClearOnExit to 0 will leave the task's notification value unchanged
 * when the function exits (in which case the value passed out in
 * pulNotificationValue will match the task's notification value).
 *
 * @param pulNotificationValue Used to pass the task's notification value out
 * of the function.  Note the value passed out will not be effected by the
 * clearing of any bits caused by ulBitsToClearOnExit being non-zero.
 *
 * @param xTicksToWait The maximum amount of time that the task should wait in
 * the Blocked state for a notification to be received, should a notification
 * not already be pending when xTaskNotifyWait() was called.  The task
 * will not consume any processing time while it is in the Blocked state.  This
 * is specified in kernel ticks, the macro pdMS_TO_TICSK( value_in_ms ) can be
 * used to convert a time specified in milliseconds to a time specified in
 * ticks.
 *
 * @return If a notification was received (including notifications that were
 * already pending when xTaskNotifyWait was called) then pdPASS is
 * returned.  Otherwise pdFAIL is returned.
 */
BaseType_t xTaskNotifyWait( 
    uint32_t ulBitsToClearOnEntry, 
    uint32_t ulBitsToClearOnExit, 
    uint32_t *pulNotificationValue, 
    TickType_t xTicksToWait );
```

*This explanation is taken from https://www.freertos.org/xTaskNotifyWait.html

xTaskNotifyWait() Return value:
* **pdTRUE** if a notification was received, or a notification was already pending when xTaskNotifyWait() was called.
* **pdFALSE** if the call to xTaskNotifyWait() timed out before a notification was received.

#### xTaskNotify()

**xTaskNotify()** is used to send an event directly to and potentially unblock an RTOS task, and optionally update the receiving task's notification value in one of the following ways:

* Write a 32-bit number to the notification value
* Add one (increment) the notification value
* Set one or more bits in the notification value
* Leave the notification value unchanged

This function must not be called from an interrupt service routine (ISR). Use xTask Notify From ISR() instead.


### 16.3. using xTaskNotifyWait() in the code

### 16.4. Task notifications  xTaskNotify()

### 16.5. Understanding Task notification trace