# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch6. Creating FreeRTOS based project for STM32 MCUs

### 6.1

system workbench for STM32 from AC6 tools

This is a tool based on eclipse but the the problem is that it doesn't support all the nucleo boards and new STM series like STM32L5.


In case you are using this tool becuase your board is supported, in new project select:
* Executable > Ac6 STM32 MCU Project

This will load al the toolchain and the proper configuration of the device.

Select: 
* Standard Peripheral Library (StdPeriph)
* Download Firmware: 'STM32F4XX_DSP_StdPeriph_Lib_V1.8.0' (example for the selected family)

 
### 6.3 including the FreeRTOS code

In the project directory
* Third-Party
    * FreeRTOS
        * org
            > here copy the source and License of FreeRTOS

Inside the folder source, the only thing that we can delete because we are not using some libraries is inside **portable**.  
Keep:
* GGC
* MemMang (memory management) 

Inside GCC keep only the directory of your own architecture


