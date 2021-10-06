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

#### 10.5.1 Real time recording (Continuous recording):

With a SEGGER **J-Link** and its Real Time Transfer (RTT) technology System View can continuously record data, and analyze and visualize it in real time.

Real time mode can be achieved via ST-link instead of J-link. For that **J-link firmware has to be flashed on ST-link circuitry of STM32 boards**.

![realtime_recording](img/realtime_recording.png)

In the custom system two layers have to been included:
* System View API
* RTT API

#### 10.5.2 Single-shot recording

**You need not to have JLINK or STLINK debugger** for this.

In single-shot mode the recording is started manually in the application, which allows recording only specific parts, which are of interest.

![single_shoot_recording](img/single_shoot_recording.png)

When no J-Link is used, SEGGER System View can be used to record data until its target buffer is filled (RTT Buffer). In single-shot mode the recording is started manually in the application, which allows recording only specific parts, which are of interest.

### 10.6. SEGGER SystemView Target integration preparation

For the single-shot recording:
* The applicartion is ready
* The RTOS is included in the project
* So we need to include APIs in our target application


#### STEP 1: Including SEGGER System View in the application

1. Download Systemview target sources and extract it
2. Create the folders as below in your FreeRTOS Project.
```c
Third-Party > SEGGER > <sources>   
```
Folders:
* Config
* OS
* Patch
* SEGGER

Inside Config, place:
* Global.h
* SEGGER_RTT_Conf.h
* SEGGER_SYSVIEW_Conf.h
* SEGGER_SYSVIEW_Confing_FreeRTOS.c

Inside OS, place:
* SEGGER_SYSVIEW_FreeRTOS.c
* SEGGER_SYSVIEW_FreeRTOS.h

Inside Patch, place a new folder with the current version of FreeRTOS: 
* FreeRTOSv10.2.1
And inside place the file:
* FreeRTOSV10_Core.patch

Inside SEGGER, place:
* SEGGER.h
* SEGGER_RTT.c
* SEGGER_RTT.h
* SEGGER_SYSVIEW.c
* SEGGER_SYSVIEW.h
* SEGGER_SYSVIEW_ConfDefaults.h
* SEGGER_SYSVIEW_Int.h

**Note:**  

For the current version I also included:
* folder Syscall (here I kept only the file for GCC)
* SEGGER_RTT_ASM_ARMv7M.S
* SEGGER_RTT_printf.c

3. Do the path settings for the include files of SEGGER in Eclipse

### 10.7. SEGGER SystemView Target integration step by step

Go to the eclipse project, refresh the Third-Party directory and check that the SEGGER subdirectory is not excluded from build (under C7C++ build properties)

```c
project > properties

C/C++ Build > Settings > MCU GCC Compiler > Includes

add workspace :
    * SEGGER/Config
    * SEGGER/OS
    * SEGGER/SEGGER
```

#### STEP 2: Patching FreeRTOS files

You need to patch some of the FreeRTOS files with patch file given by SEGGER system View

right click on FreeRTOS folder (in eclipse)
* Team 
    * Apply patch

### 10.8. SEGGER SystemView taking FreeRTOS trace using snapshot mode


### 10.9. SEGGER SystemView jlink reflash utility download


### 10.10. SEGGER SystemView taking FreeRTOS trace using Continuous recording


### 10.11. Analyzing the trace  An Overview


### 10.12. Analyzing FreeRTOS helloword application using SystemView Trace
