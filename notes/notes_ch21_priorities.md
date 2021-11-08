# Course: Mastering RTOS: Hands on FreeRTOS and STM32Fx with Debugging

Notes and Code Author: Leandro D. Medus  
August 2021

## Ch21. ARM Cortex M Interrupt Priority and  FreeRTOS Task Priority

### 21.1. FreeRTOS Hardware interrupt Configurable items


* ConfigKERNEL_INTERRUPT_PRIORITY 
* configMAX_SYSCALL_INTERRUPT_PRIORITY


### 21.2. FreeRTOS Hardware interrupt Co nfigurable items contd.

### 21.3. Priority of freeRTOS Tasks

### 21.4. FreeRTOS priority SetGet APIs

### 21.5. Exercise  Task Priority

### 21.6. Exercise  Task Priority Implementation and testing

### 21.7. Exercise  Task Priority analyzing the trace


En la siguiente imagen se ve claramente como luego del cambio de prioridade (menos de 100 us) se produce el cambio de contexto.

![log_cambio_prioridades](img/log_cambio_prioridades.png)

El problema está en el tiempo de demora que hay desde que se produce la interrupción hasta que se ejecuta el cambio de contexto. Esto es de entre 300 ms y 500 ms y esto se condice a como está programada la aplicación. 

Esto se puede ver dado que que si la verificación del flag del cambio de prioridad se hace a intervalos determinado por los delay que corren como dummy en el procesador, esto afectará el tiempo de muestreo.

![log_cambio_prioridades_300ms](img/log_cambio_prioridades_300ms.png)

Una modificación no óptima pero efectiva, solo a efectos de mantener la prueba de concepto es en los delays, hacer la verificación del flag del cambio de prioridades. Así se puede observar que entre la ISR y el cambio de tarea pasan menos de 250 us.

![log_cambio_prioridades_Correcto](img/log_cambio_prioridades_Correcto.png)

