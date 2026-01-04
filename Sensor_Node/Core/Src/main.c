/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint32_t ic1_rise, ic1_fall;
uint32_t ic2_rise, ic2_fall;

uint8_t echo1_state = 0;
uint8_t echo2_state = 0;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void TRIG1_Pulse(void)
{
    HAL_GPIO_WritePin(TRIG1_GPIO_Port, TRIG1_Pin, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(TRIG1_GPIO_Port, TRIG1_Pin, GPIO_PIN_RESET);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void CAN_Send_Distance(uint8_t sensor_id, uint16_t distance) {
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8];
    uint32_t txMailbox;

    txHeader.StdId = 0x123; // 원하는 ID 설정
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.IDE = CAN_ID_STD;
    txHeader.DLC = 3; // 1바이트(ID) + 2바이트(거리)
    txHeader.TransmitGlobalTime = DISABLE;

    txData[0] = sensor_id;
    txData[1] = (distance >> 8) & 0xFF; // 상위 바이트
    txData[2] = distance & 0xFF;        // 하위 바이트

    if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox) != HAL_OK) {
        // 전송 오류 처리
    }
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {
      // 1. 센서 1 트리거 및 거리 계산
      TRIG1_Pulse();
      HAL_Delay(60); // 센서 간 간섭 방지를 위한 대기

      if (echo1_state == 2) {
          uint32_t diff1 = calc_diff(ic1_rise, ic1_fall);
          float distance1 = (diff1 * 0.034) / 2; // 타이머가 1MHz(1us)로 설정되었다는 가정 하에

          // CAN 전송 함수 호출 (아래 정의 필요)
          CAN_Send_Distance(1, (uint16_t)distance1);

          echo1_state = 0; // 상태 초기화
      }

      // 2. 센서 2 트리거 및 거리 계산
      HAL_Delay(100); // 전체 주기 조절
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM1) return;

    /* Sensor 1 */
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        if (echo1_state == 0)
        {
            ic1_rise = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
            echo1_state = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim,
                TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            ic1_fall = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
            echo1_state = 2;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim,
                TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
        }
    }

    /* Sensor 2 */
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
    {
        if (echo2_state == 0)
        {
            ic2_rise = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
            echo2_state = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim,
                TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            ic2_fall = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
            echo2_state = 2;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim,
                TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
        }
    }
}


uint32_t calc_diff(uint32_t rise, uint32_t fall)
{
    if (fall >= rise) return fall - rise;
    return (0xFFFF - rise) + fall;
}


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
