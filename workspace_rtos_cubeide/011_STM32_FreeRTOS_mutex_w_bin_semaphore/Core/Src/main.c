/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Example 010: Counting semaphore in FreeRTOS for NUCLEO-L552.
 *
 * @details There are two tasks:
 *              1. Handler task
 *              2. Periodic task
 *
 * A Counting semaphore was used to process latched events (produced by the
 * handler task) sent by fast triggering interrupts.
 *
 * Note: The periodic task priority is higher than the handler task priority .
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright 2021 Leandro Medus.
 * All rights reserved.</center></h2>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************
 */

/*
 * Other useful functions:
 *
 *      SEGGER_SYSVIEW_PrintfTarget("Task-1 is yielding");
 *      traceISR_ENTER();
 *      traceISR_EXIT();
 *      traceISR_EXIT_TO_SCHEDULER();
 *      taskYIELD();
 * */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TRUE                1
#define FALSE               0

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT        ( 0xfffff )

#define DWT_CTRL    (*(volatile uint32_t*)0xE0001000)       /* Data Watchpoint and Trace Unit */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef hlpuart1;

/* USER CODE BEGIN PV */

char usr_msg[250] = {0};

TaskHandle_t xTaskHandleM = NULL;
TaskHandle_t xTaskHandleE = NULL;

//binary semaphore handle
SemaphoreHandle_t xBinarySemaphore;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
/* USER CODE BEGIN PFP */

void sendString(char *msg);
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */
    BaseType_t status;

    char msg_program_init [] = "[info] ---- Example 011: Mutex using binary semaphore in FreeRTOS for NUCLEO-L552. ----\r\n";

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_LPUART1_UART_Init();
    /* USER CODE BEGIN 2 */

    DWT_CTRL |= ( 1 << 0);                      // Enable the CYCCNT counter. (to maintain time stamps in Segger)

    sendString(msg_program_init);               // send program description with UART

    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();                     // Start recording with SEGGER

    // Creating a binary semaphore
    vSemaphoreCreateBinary(xBinarySemaphore);

    if(xBinarySemaphore != NULL)
    {
        /* Create one of the two tasks. */
        status = xTaskCreate(    vTask1,     /* Pointer to the function that implements the task. */
                "Task 1",   /* Text name for the task.  This is to facilitate debugging only. */
                500,        /* Stack depth in words. */
                NULL,       /* We are not using the task parameter. */
                1,          /* This task will run at priority 1. */
                NULL );     /* We are not using the task handle. */

        configASSERT(status == pdPASS);

        /* Create the other task in exactly the same way. */
        status = xTaskCreate( vTask2, "Task 2", 500, NULL, 1, NULL );

        configASSERT(status == pdPASS);

        // make semaphore available for the first time
        xSemaphoreGive(xBinarySemaphore);

        /* Start the scheduler so our tasks start executing. */
        vTaskStartScheduler();
    }else
    {
        sprintf(usr_msg,"binary semaphore creation failed\r\n");
        sendString(usr_msg);

    }

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* If all is well we will never reach here as the scheduler will now be
        running the tasks.  If we do reach here then it is likely that there was
        insufficient heap memory available for a resource to be created. */

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 55;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief LPUART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_LPUART1_UART_Init(void)
{

    /* USER CODE BEGIN LPUART1_Init 0 */

    /* USER CODE END LPUART1_Init 0 */

    /* USER CODE BEGIN LPUART1_Init 1 */

    /* USER CODE END LPUART1_Init 1 */
    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 115200;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
    if (HAL_UART_Init(&hlpuart1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN LPUART1_Init 2 */

    // enabling interrupt for the flag
    __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);


    /* USER CODE END LPUART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : LED_GREEN_Pin */
    GPIO_InitStruct.Pin = LED_GREEN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GREEN_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : LED_RED_Pin */
    GPIO_InitStruct.Pin = LED_RED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : LED_BLUE_Pin */
    GPIO_InitStruct.Pin = LED_BLUE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_BLUE_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* --------------------------------------------------------------------------*/

/**
 * @brief  This function send a string using the UART
 * @retval None
 */
void sendString(char *msg)
{
    sprintf(usr_msg, msg);

    // sending a string
    HAL_UART_Transmit(&hlpuart1, (uint8_t *) usr_msg, strlen(usr_msg), 100);
}

/**
 * @brief
 * @details
 *
 * @retval None
 */
void vTask1( void *pvParameters )
{
    const char *pcTaskName = "Task 1 is running\r\n";

    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ;; )
    {

        //before printing , lets own the semaphore or take the semaphore */
        xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );

        /* Print out the name of this task. */
        sprintf( usr_msg,"%s", pcTaskName);
        sendString(usr_msg);

        //give the semaphore here. give operation increases the bin sema value back to 1
        xSemaphoreGive(xBinarySemaphore);

        /*Now this task will be blocked for 500ticks */
        vTaskDelay( pdMS_TO_TICKS(500) );
    }
}

/**
 * @brief
 * @details
 *
 * @retval None
 */
void vTask2( void *pvParameters )
{

    const char *pcTaskName = "Task 2 is running\r\n";

    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ;; )
    {

        //before printing , lets own the semaphore or take the semaphore */
        xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );

        /* Print out the name of this task. */
        sprintf( usr_msg,"%s", pcTaskName);
        sendString(usr_msg);

        //give the semaphore here. give operation increases the bin sema value back to 1
        xSemaphoreGive(xBinarySemaphore);

        /*Now this task will be blocked for 500ticks */
        vTaskDelay( pdMS_TO_TICKS(500));
    }
}

/* --------------------------------------------------------------------------*/
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
        __NOP();
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
