# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch10. 10. FreeRTOS app debugging using SEGGER SystemView Tools


![semihosting_overview](img/semihosting_overview.png)


### 10.2. Downloading SEGGER SystemView Software

Download: 
* SEGGER SystemView (windows installer)
* SystemView Target sources 
* Manuals

### 10.3. SEGGER SystemView Installation and first look

Just intalling SystemView_Windows_V330_x64.exe for a fresh install.

### 10.4. What is SEGGER SystemView 

System View is a software toolkit which is used to analyze the embedded software behaviour running on your target.

The embedded software may contain embedded OS or RTOS or it could be non-OS based application.

**The system View can be used to analyze how your embedded code is behaving on the target** 

In the case of a FreeRTOS application:
* You can analyze how many tasks are running and how much duration they consume on the CPU.
* ISR entry and exit timings and duration of run on the CPU.  
* You can analyze other behaviour of tasks: like blocking, unblocking, notifying, yielding, etc.
* You can analyze CPU idle time so that you can think of sending CPU to speed mode
* Total runtime behaviour of the application.


* It sheds light on what exactly happened in which order, which interrupt has triggered which task switch, which interrupt and task has called which API function of the underlying RTOS.
* System View should be used to verify that the embedded system behaves as expected and can be used to find problems and inefficiencies, such as superfluous and spurious interrupts, and unexpected task changes.


#### Toolkit

SystemViewer toolkit come in two parts:
1. PC visualization software: System View Host software (Windows/Linux/mac)
2. System View target codes (this is used to collect the target events and sending back to PC visualization software)


### 10.5. SEGGER SystemView visualization modes

1. Real time recording (Continuous recording):

With a SEGGER J-Link and its Real Time Transfer (RTT) technology System View can continuously record data, and analyze and visualize it in real time.

Real time mode can be achieved via ST-link instead of J-link. For that J-link firmware has to be flashed on ST-link circuitry of STM32 boards. More on this later.

### 10.6. SEGGER SystemView Target integration preparation


### 10.7. SEGGER SystemView Target integration step by step


### 10.8. SEGGER SystemView taking FreeRTOS trace using snapshot mode


### 10.9. SEGGER SystemView jlink reflash utility download


### 10.10. SEGGER SystemView taking FreeRTOS trace using Continuous recording


### 10.11. Analyzing the trace  An Overview


### 10.12. Analyzing FreeRTOS helloword application using SystemView Trace
