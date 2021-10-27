# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch19. FreeRTOS Kernel Coding Style

From the website: **Coding Standard, Testing and Style Guide**

[https://www.freertos.org/FreeRTOS-Coding-Standard-and-Style-Guide.html](https://www.freertos.org/FreeRTOS-Coding-Standard-and-Style-Guide.html)



### 19.1. Macros

Macros are pre-fixed with the file in which they are defined. The pre-fix is lower case. For example, **configUSE_PREEMPTION** is defined in FreeRTOSConfig.h.

the prefix **config** indicates in which file the macro is defined.

Other than the pre-fix, macros are written in all upper case, and use an underscore to separate words.

Similarly, 

**portmacro.h**: header file which contains macros related to architecture specific data. So, FreeRTOS use as prefix **port**. Ex:

```c
#define portCHAR   char

```

### 19.2. Variables Convention

* Variables of type uint32_t are prefixed **ul**, where the 'u' denotes 'unsigned' and the 'l' denotes 'long'.
* Variables of type uint16_t are prefixed **us**, where the 'u' denotes 'unsigned' and the 's' denotes 'short'.
* Variables of type uint8_t are prefixed **uc**, where the 'u' denotes 'unsigned' and the 'c' denotes 'char'.
* Variables of **non stdint types** are prefixed **x**. Examples include BaseType_t and TickType_t, which are portable layer defined typedefs for the natural or most efficient type for the architecture and the type used to hold the RTOS tick count respectively.
* **Unsigned variables of non stdint types** have an additional prefix u. For example variables of type UBaseType_t (unsigned BaseType_t) are prefixed **ux**.
* Variables of type **size_t** are also prefixed **x**.>
* Enumerated variables are prefixed **e**
* Pointers have an additional prefixed **p**, for example a pointer to a uint16_t will have prefix **pus**.
* In line with MISRA guides, unqualified standard char types are only permitted to hold ASCII characters and are prefixed **c**.
* In line with MISRA guides, variables of type **char \*** are only permitted to hold pointers to ASCII strings and are prefixed **pc**.


### 19.3. Functions

* **File scope static** (private) functions are prefixed with **prv**.
* **API functions are prefixed with their return type**, as per the convention defined for variables, with the addition of the prefix **v** for void.
* API function **names start with the name of the file** in which they are defined. For example **vTaskDelete is defined in tasks.c**, and has a void return type.
