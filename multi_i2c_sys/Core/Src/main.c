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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "ads1115.h"
#include "tca9548a.h"
#include "motoron.h"

//#include <strings.h>

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


static const uint32_t TIMEOUT_I2C  = 50U;
static const uint32_t TIMEOUT_UART = 50U;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  char uart_tx[32];

  HAL_UART_Transmit(&huart1, (uint8_t*)"\r\nProgram Started!\r\n", strlen("\r\nProgram Started!\r\n"), 100);

  for (uint8_t addr = 1; addr < 128; addr++)
  {
      if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK)
      {
          sprintf(uart_tx, "\r\nI2C BUS Devices: 0x%02X\r\n", addr);
          HAL_UART_Transmit(&huart1, (uint8_t*)uart_tx, strlen(uart_tx), 100);
      }
  }



  HAL_Delay(2000);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  i2c_motoron(&hi2c1 , &huart1);
  hal_i2c_motoron_initialization(&hi2c1 , &huart1);

  float voltage = 0;
  int16_t adc_raw = 0;
  uint8_t i2c_rx_buff[2] = {0};

  PotResult pot_to_speed;

  while (1)
  {

	  char uart_tx_i2c[80];

	  HAL_StatusTypeDef status;


	  for(uint8_t mux_ch = 0; mux_ch < 8 ; mux_ch ++)
	  {
		  status = i2c_mux_module(&hi2c1 , &huart1 , MULPLXR_CH_MASK[0] , CH_I2C_TX_BUFF[0] ,
		  					  	  	  	  	  ADC_CONFIG_SIZE , ADC_RX_SIZE, i2c_rx_buff , 0);

		  if (status == HAL_OK)
		  {
			  pot_to_speed = potentiometer_to_speed(i2c_rx_buff);

			  i2c_motoron_motor_set(&hi2c1 , &huart1 ,  i2c_rx_buff , pot_to_speed.speed);
		  }



		  sprintf(uart_tx_i2c , "\n\rMux[%d] , ADC%d: 0x%04x , voltage: %.4f v" , mux_ch , 0  , pot_to_speed.adc_raw, pot_to_speed.voltage_mv / 1000.0);
		  HAL_UART_Transmit(&huart1, uart_tx_i2c, strlen(uart_tx_i2c), 100);


		  for(uint8_t adc_ch = 1; adc_ch < 4 ; adc_ch++)
		  {
				  status = i2c_mux_module(&hi2c1 , &huart1 , MULPLXR_CH_MASK[mux_ch] , CH_I2C_TX_BUFF[adc_ch] ,
										  ADC_CONFIG_SIZE , ADC_RX_SIZE, i2c_rx_buff , adc_ch);


				  if(status != HAL_OK)
				  {
	//				  snprintf(uart_tx_i2c ,  sizeof(uart_tx_i2c) , " \r\nMux[%d], ADC[%d] not working, Hal status: %d" , mux_ch , adc_ch, status);
	//				  HAL_UART_Transmit(&huart1, uart_tx_i2c, strlen(uart_tx_i2c), 50);
					  continue;
				  }

		  }



		  }


//			  HAL_Delay(10);





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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
#ifdef USE_FULL_ASSERT
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
