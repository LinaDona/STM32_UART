/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "icache.h"
#include "memorymap.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
HAL_StatusTypeDef HAL_UART_ReceiveString(
        UART_HandleTypeDef *huart, uint8_t *pData,
        uint16_t Size, uint32_t Timeout) {
    const char newline[] = "\r\n";
    const char delete[] = "\x08 \x08";
    HAL_StatusTypeDef status;

    if(Size == 0)
        return HAL_ERROR;

    int i = 0;
    for(;;) {
        status = HAL_UART_Receive(huart, &pData[i], 1, Timeout);
        if(status != HAL_OK)
            return status;

        if((pData[i] == '\x08')||(pData[i] == '\x7F')) { // backspace
            if(i > 0) {
                status = HAL_UART_Transmit(huart, (uint8_t*)delete,
                                           sizeof(delete)-1, Timeout);
                if(status != HAL_OK)
                    return status;
                i--;
            }
            continue;
        }

        if((pData[i] == '\r') || (pData[i] == '\n')) {
            pData[i] = '\0';
            status = HAL_UART_Transmit(huart, (uint8_t*)newline,
                                       sizeof(newline)-1, Timeout);
            if(status != HAL_OK)
                return status;
            break;
        }

        // last character is reserved for '\0', ignore
        if(i == (Size-1))
            continue;

        status = HAL_UART_Transmit(huart, &pData[i], 1, Timeout);
        if(status != HAL_OK)
            return status;
        i++;
    }

    return HAL_OK;
}

void error(void) {
    HAL_Delay(HAL_MAX_DELAY);
}

void init(void) {
    const char ready[] = "Ready!\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)ready,
                      sizeof(ready)-1, HAL_MAX_DELAY);
}

void loop(void) {
    HAL_StatusTypeDef status;
    const char question[] = "What is your name?\r\n";
    char answer[256];
    char name[32];

    status = HAL_UART_Transmit(&huart3, (uint8_t*)question,
                               sizeof(question)-1, HAL_MAX_DELAY);
    if(status != HAL_OK)
        error();

    status = HAL_UART_ReceiveString(&huart3, (uint8_t*)name,
                                    sizeof(name), HAL_MAX_DELAY);
    if(status != HAL_OK)
        error();

    int code = snprintf(answer, sizeof(answer),
                        "Hello, %s!\r\n", name);
    if(code < 0)
        error();

    status = HAL_UART_Transmit(&huart3, (uint8_t*)answer,
                               strlen(answer), HAL_MAX_DELAY);
    if(status != HAL_OK)
        error();

    HAL_Delay(100);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the System Power */
  SystemPower_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN BSP */

  /* -- Sample board code to send message over COM1 port ---- */

  /* USER CODE END BSP */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  init();
  while (1)
  {

    /* USER CODE END WHILE */
	  loop();
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/* USER CODE BEGIN 4 */

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
