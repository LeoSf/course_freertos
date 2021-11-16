/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Example 008: Queue Processing in FreeRTOS for NUCLEO-L552.
 *
 * @details FreeRTOS application which implements the below commands
 *          LED_ON,
 *          LED_OFF,
 *          LED_TOGGLE_START
 *          LED_TOGGLE_STOP
 *          LED_STATUS_READ
 *          RTC_DATETIME_READ
 *
 *          The command should be sent to the board via UART from the user.
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
 *      snprintf(msg, 100, "%s\n", (char*)parameters);
 *      sendString(msg);
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
#include "timers.h"

#include <stdio.h>
#include <string.h>
//#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TRUE                1
#define FALSE               0

#define AVAILABLE           TRUE
#define NOT_AVAILABLE       FALSE

#define PRESSED             TRUE
#define NOT_PRESSED         FALSE

// App commands
#define LED_ON_COMMAND              1
#define LED_OFF_COMMAND             2
#define LED_TOGGLE_COMMAND          3
#define LED_TOGGLE_STOP_COMMAND     4
#define LED_READ_STATUS_COMMAND     5
#define RTC_READ_DATE_TIME_COMMAND  6

#define DEBOUNCE_DELAY_MS   10

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

uint8_t UART_ACCESS_KEY = AVAILABLE;
uint8_t button_status_flag = NOT_PRESSED;

// task handles
TaskHandle_t xTask_1_handle = NULL;
TaskHandle_t xTask_2_handle = NULL;
TaskHandle_t xTask_3_handle = NULL;
TaskHandle_t xTask_4_handle = NULL;

// Queue handle
QueueHandle_t command_queue = NULL;
QueueHandle_t uart_write_queue = NULL;

// Software timer handler
TimerHandle_t led_timer_handle = NULL;

// Command structure
typedef struct APP_CMD
{
    uint8_t COMMAND_NUM;            // command number of the user
    uint8_t COMMAND_ARGS[10];       // associated argument
}APP_CMD_t;

uint8_t command_buffer[20];
uint8_t command_len = 0;

//This is the menu
char menu[]={"\
\r\nLED_ON             ----> 1 \
\r\nLED_OFF            ----> 2 \
\r\nLED_TOGGLE         ----> 3 \
\r\nLED_TOGGLE_OFF     ----> 4 \
\r\nLED_READ_STATUS    ----> 5 \
\r\nRTC_PRINT_DATETIME ----> 6 \
\r\nEXIT_APP           ----> 0 \
\r\nType your option here : "};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
/* USER CODE BEGIN PFP */

void sendString(char *msg);
static void vtask_1_menu_display(void* parameters);
static void vtask_2_cmd_handling(void* parameters);
static void vtask_3_cmd_processing(void * parameters);
static void vtask_4_uart_write(void * parameters);

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

    char msg_program_init [] = "[info] ---- Example 008: Queue Processing in FreeRTOS for NUCLEO-L552. ----\r\n";

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

    sendString(msg_program_init);

    SEGGER_SYSVIEW_Conf();

    SEGGER_SYSVIEW_Start();                     // Start recording with SEGGER

    status = xTaskCreate(
            vtask_1_menu_display,           // name of the task handler
            "TASK-1-MENU",                  // descriptive name. (Could be NULL)
            configMINIMAL_STACK_SIZE,       // stack space ([words] = 4*words [bytes])
            "Task-1 [info]",                // pvParameters
            1,                              // priority of the task
            &xTask_1_handle);               // handler to the TCB (task controller block)

    configASSERT(status == pdPASS);

    status = xTaskCreate(
            vtask_2_cmd_handling,
            "TASK-2-CMD-HANDLING",
            configMINIMAL_STACK_SIZE,
            "Task-2 [info]",
            1,
            &xTask_2_handle);

    configASSERT(status == pdPASS);

    status = xTaskCreate(
            vtask_3_cmd_processing,
            "TASK-3-CMD-PROCESS",
            configMINIMAL_STACK_SIZE,
            "Task-3 [info]",
            1,
            &xTask_3_handle);

    configASSERT(status == pdPASS);

    status = xTaskCreate(
            vtask_4_uart_write,
            "TASK-4-UART-WRITE",
            configMINIMAL_STACK_SIZE,
            "Task-4 [info]",
            1,
            &xTask_4_handle);

    configASSERT(status == pdPASS);


    // Command Queue
    command_queue = xQueueCreate( 10, sizeof(APP_CMD_t *));     // a memory pointer is 4 bytes = 40 bytes instead 110

    // Write Queue
    uart_write_queue = xQueueCreate( 10, sizeof(char *));          // we only need only the staring addr of the msg

    if(command_queue != NULL && uart_write_queue != NULL)
    {
        vTaskStartScheduler();                  // start the freeRTOS scheduler
    }
    else
    {
        sprintf(usr_msg,"Queue creation failed\r\n");
        sendString(usr_msg);
    }





    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* Execution must not reach this point */

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
static void vtask_1_menu_display(void* parameters)
{
    while(TRUE)
    {

    }
}

/**
 * @brief
 * @details
 *
 * @retval None
 */
static void vtask_2_cmd_handling(void* parameters)
{
    while(TRUE)
    {

    }
}

/**
 * @brief
 * @details
 *
 * @retval None
 */
static void vtask_3_cmd_processing(void * parameters)
{
    while(TRUE)
    {

    }
}

/**
 * @brief
 * @details
 *
 * @retval None
 */
static void vtask_4_uart_write(void * parameters)
{
    while(TRUE)
    {

    }
}


/**
 * @brief
 * @details
 *
 * @retval None
 */
void vApplicationIdleHook(void)
{
    /*
     * Remember that when you send the uC to sleep mode, you can't get a trace.
     */
}



void LPUART_ISR(void)
{
    uint16_t data_byte;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if( __HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_RXNE) )     // a data byte is received from the user
    {
        data_byte = (uint8_t) hlpuart1.Instance->RDR;

        command_buffer[command_len++] = (data_byte & 0xFF) ;

        if(data_byte == '\r')       // user finished entering the data
        {
            command_len = 0;        // reset the command_len variable

            // lets notify the command handling task
            xTaskNotifyFromISR(xTask_2_handle, 0, eNoAction, &xHigherPriorityTaskWoken);
            xTaskNotifyFromISR(xTask_1_handle, 0, eNoAction, &xHigherPriorityTaskWoken);
        }
    }

    // if the above freertos apis wake up any higher priority task, then yield the processor to the
    // higher priority task which is just woken up.

    if(xHigherPriorityTaskWoken)
    {
        taskYIELD();
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
