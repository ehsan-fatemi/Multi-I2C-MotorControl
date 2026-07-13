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

  HAL_UART_Transmit(&huart1, (uint8_t*)"UART1_test\r\n", strlen("UART1_test\r\n"), 100);

  for (uint8_t addr = 1; addr < 128; addr++)
  {
      if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK)
      {
          sprintf(uart_tx, "addr: 0x%02X\r\n", addr);
          HAL_UART_Transmit(&huart1, (uint8_t*)uart_tx, strlen(uart_tx), 100);
      }
  }

//  HAL_I2C_Master_Transmit(&hi2c1, 0x49, 5, 1, 100);
  //  HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout)


  HAL_Delay(3000);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  char uart_tx_i2c[80];

	  uint8_t target_addr = 0x48;
	  uint8_t conf_reg_addr = 0x01;

//	  uint16_t conf_reg_data = 0x280; // 0b00000010  10000000;
//	  uint8_t conf_reg_data_arr[] = {0x02 , 0x80}; // 0b00000010  10000000;
//	  uint8_t conf_reg_data_MSB = 0x02;
//	  uint8_t conf_reg_data_LSB = 0x80;

	  // FSR (PGA) = 001 , MUX (Input multiplexer configuration) = 100
	  uint16_t conf_reg_data = 0x4280; // 0b01000010  10000000;
	  uint8_t conf_reg_data_arr[] = {0x42 , 0x80}; // 0b01000010  10000000;
	  uint8_t conf_reg_data_MSB = 0x42;
	  uint8_t conf_reg_data_LSB = 0x80;



//	  HAL_I2C_Master_Transmit(&hi2c1, target_addr << 1 , &conf_reg_addr, 1, 100);
//	  HAL_I2C_Master_Transmit(&hi2c1, target_addr << 1 , &conf_reg_data_arr, 2, 100);

	  // To write to the sensor

	  uint8_t i2c_tx_buff[] = {conf_reg_addr , conf_reg_data_MSB , conf_reg_data_LSB};
	  HAL_I2C_Master_Transmit(&hi2c1, target_addr << 1 , i2c_tx_buff, 3, 100);

	  sprintf(uart_tx_i2c , "\n\rWrite: conf_reg: 0x%x , conf_data: 0x%x , 0x%x" ,i2c_tx_buff[0] , i2c_tx_buff[1] ,i2c_tx_buff[2] );
	  HAL_UART_Transmit(&huart1, uart_tx_i2c, strlen(uart_tx_i2c), 100);

//	  HAL_I2C_Mem_Write(&hi2c1, target_addr << 1 , 0x01, 1, &conf_reg_data_arr, 2, 100);

	  HAL_Delay(1000);

	  // To read from the sensor
	  uint8_t conv_reg_addr = 0x00;
	  HAL_I2C_Master_Transmit(&hi2c1, target_addr << 1, &conv_reg_addr, 1, 100);
	  uint8_t i2c_rx_buf[2];
	  HAL_I2C_Master_Receive(&hi2c1, target_addr << 1 , i2c_rx_buf, 2, 100);

	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

	  int16_t adc_raw = (int16_t )((i2c_rx_buf[0] << 8) | i2c_rx_buf[1]);

	  // to cancel the noise effect on the print:
	  if(adc_raw < 0)
	  {
		  adc_raw = 0;
	  }
	  float voltage = adc_raw * 4.096 / 32768.0f;
//	  sprintf(uart_tx_i2c , "\n\rADC0: 0.x%02x%02x , voltage: %f v" , i2c_rx_buf[0] , i2c_rx_buf[1], voltage);
	  sprintf(uart_tx_i2c , "\n\rADC0: 0.x%02x , voltage: %f v" , adc_raw, voltage);
	  HAL_UART_Transmit(&huart1, uart_tx_i2c, strlen(uart_tx_i2c), 100);

//	  HAL_UART_Transmit(&huart1, (uint8_t*)"\r\nUART1_test", 12 , 100);
	  HAL_Delay(100);

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
