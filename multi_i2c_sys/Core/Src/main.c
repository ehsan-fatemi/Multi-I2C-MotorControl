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
//#include <strings.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define ADC_CONFIG_SIZE 3U
#define ADC_RX_SIZE     2U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static const uint8_t CONV_REG_ADDR = 0x00;

static const uint8_t MULPLXR_ADDR = 0x70;
static const uint8_t ADC_ADDR     = 0x48;

static const uint8_t CH_I2C_TX_BUFF[][3] = {
		{0x01, 0xC2, 0x80},
		{0x01, 0xD2, 0x80},
		{0x01, 0xE2, 0x80},
		{0x01, 0xF2, 0x80}
};

static const uint8_t MULPLXR_CH_MASK[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

static const uint32_t TIMEOUT_I2C  = 50U;
static const uint32_t TIMEOUT_UART = 50U;

static const uint8_t MOTORUN_ADDR = 0x10;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
HAL_StatusTypeDef i2c_adc_module(uint8_t adc_channel , const uint8_t i2c_tx_buff[] , uint8_t tx_arr_size  ,  uint8_t * i2c_rx_buff , uint8_t rx_arr_size)
  {


	  HAL_StatusTypeDef status;

	  status = HAL_I2C_Master_Transmit(&hi2c1, ADC_ADDR << 1 , i2c_tx_buff, tx_arr_size , TIMEOUT_I2C);

	  if(status != HAL_OK)
	  {
		  return status;
	  }

	  HAL_Delay(10);

	  // To read from the sensor
	  status = HAL_I2C_Master_Transmit(&hi2c1, ADC_ADDR << 1, &CONV_REG_ADDR , 1, TIMEOUT_I2C);

	  if(status != HAL_OK)
	  	  {
	  		  return status;
	  	  }

	  HAL_Delay(10);

	  status = HAL_I2C_Master_Receive(&hi2c1, ADC_ADDR << 1 , i2c_rx_buff , rx_arr_size, TIMEOUT_I2C);

	  if(status != HAL_OK)
	  	  {
	  		  return status;
	  	  }


//  	  int16_t adc_raw = (int16_t )((i2c_rx_buff[0] << 8) | i2c_rx_buff[1]);
//
//  	  // to cancel the noise effect on the print:
//  	  if(adc_raw < 0)
//  	  {
//  		  adc_raw = 0;
//  	  }
//  	  float voltage = adc_raw * 4.096 / 32768.0f;
//	  	  char uart_tx_i2c[80];
//  	  sprintf(uart_tx_i2c , "\n\rADC%d: 0x%04x , voltage: %.4f v" ,adc_channel, adc_raw, voltage);
//  	  HAL_UART_Transmit(&huart1, uart_tx_i2c, strlen(uart_tx_i2c), 100);

	  return HAL_OK;

  }


HAL_StatusTypeDef i2c_mux_module(uint8_t mulplxr_channel , const uint8_t i2c_tx_buff[] ,
								uint8_t tx_arr_size , uint8_t rx_arr_size,
								uint8_t * i2c_rx_buff , uint8_t adc_channel
								){


//		i2c_rx_buff[0] = 0;
//		i2c_rx_buff[1] = 0;


		HAL_StatusTypeDef status;

		status = HAL_I2C_Master_Transmit(&hi2c1, MULPLXR_ADDR << 1 , &mulplxr_channel , 1 , TIMEOUT_I2C);

		if(status != HAL_OK)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nerrMULPLXR_ADDR", strlen("\r\nerrMULPLXR_ADDR"), TIMEOUT_UART);
			return status;
		}
		HAL_Delay(10);

		status = i2c_adc_module(adc_channel , i2c_tx_buff , tx_arr_size ,  i2c_rx_buff , rx_arr_size);
		if(status != HAL_OK)
		{
			return status;
		}



	return HAL_OK;
}

HAL_StatusTypeDef i2c_motoron_motor_set(uint8_t * i2c_rx_buff_pot)
{
	HAL_StatusTypeDef status;
	char uart_tx[100];

	uint8_t motoron_set_speed[4] ;//= {0xD1 , 0x01 , 0x00 , 0x40};

	snprintf(uart_tx , sizeof(uart_tx) , "\r\nPotentiometer: 0x%02x 0x%02x" , i2c_rx_buff_pot[0] ,i2c_rx_buff_pot[1]);
	HAL_UART_Transmit(&huart1, (uint8_t *) uart_tx, strlen(uart_tx), TIMEOUT_UART);

//	uint8_t motor = 0x01;
//	uint16_t speed = 0x672;
//	motoron_set_speed[0] = 0xD1;
//	motoron_set_speed[1] = motor & 0x7F;
//	motoron_set_speed[2] = speed & 0x7F;
//	motoron_set_speed[3] = (speed >> 7) & 0x7F;

	int16_t adc_raw  = (int16_t)((i2c_rx_buff_pot[0] << 8 ) | (i2c_rx_buff_pot[1]));
	if(adc_raw < 0){adc_raw = 0;}
//	adc_raw = adc_raw  / 41;		// maps 0..32767 to 0..799
	int16_t speed = (int16_t)(((int32_t)adc_raw * 1600) / 32767 - 800);
	if (speed > 800)
	    speed = 800;

	if (speed < -800)
	    speed = -800;
	uint8_t motor = 0x01;
	motoron_set_speed[0] = 0xD1;
	motoron_set_speed[1] = motor & 0x7F;
	motoron_set_speed[2] = speed & 0x7F;
	motoron_set_speed[3] = (speed >> 7) & 0x7F;

	snprintf(uart_tx , sizeof(uart_tx) , "\r\nmotoron_set_speed: 0x%04x 0x%04x 0x%04x 0x%04x speed:%d " , motoron_set_speed[0] ,motoron_set_speed[1] , motoron_set_speed[2] , motoron_set_speed[3] , speed);
	HAL_UART_Transmit(&huart1, (uint8_t *) uart_tx, strlen(uart_tx), TIMEOUT_UART);


	status = HAL_I2C_Master_Transmit(&hi2c1, MOTORUN_ADDR << 1 , motoron_set_speed, 4 , TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		uint32_t err = HAL_I2C_GetError(&hi2c1);

		snprintf(uart_tx, sizeof(uart_tx),
		         "\r\nI2C failed. status=%d error=0x%08lX state=%d",
		         status, err, hi2c1.State);

		HAL_UART_Transmit(&huart1, (uint8_t *)uart_tx, strlen(uart_tx), TIMEOUT_UART);

//		snprintf(uart_tx , sizeof(uart_tx) , "\r\nerror motoron_set_speed!" );
//		HAL_UART_Transmit(&huart1, (uint8_t *) uart_tx, strlen(uart_tx), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

//	HAL_UART_Transmit(&huart1, (uint8_t *) "\r\nmotor set", strlen("\r\nmotor set"), TIMEOUT_UART);
	return status;
}

HAL_StatusTypeDef hal_i2c_motoron_initialization()
{
	HAL_StatusTypeDef status;
	char uart_tx[100];
	uint8_t motoron_clear_motor_fault = 0xA6;

	status = HAL_I2C_Master_Transmit(&hi2c1, MOTORUN_ADDR << 1 , &motoron_clear_motor_fault, 1, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nerror motoron_clear_motor_fault", strlen("\r\nerror motoron_clear_motor_fault"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

	uint8_t motoron_clear_latched_status_flags[] = {0xA9 , 0x00 , 0x04};
	status = HAL_I2C_Master_Transmit(&hi2c1, MOTORUN_ADDR << 1 , motoron_clear_latched_status_flags, 3, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nerror motoron_clear_latched_status_flags", strlen("\r\nerror motoron_clear_latched_status_flags"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

	return status;

}

HAL_StatusTypeDef i2c_motoron()
{
	char uart_tx[100];

	static const uint8_t Reinitialize_Reg[] = {0x96 , 0x74};
	static const uint8_t CRC_DISABLE[] = {0x8B , 0x04 , 0x7B , 0x43};
	static const uint8_t MOTORN_VERSION 	= 0x87;
	uint8_t MOTORUN_RX_BUFFER[4] = {0};

	HAL_StatusTypeDef status;
	status = HAL_I2C_Master_Transmit(&hi2c1, MOTORUN_ADDR << 1 , Reinitialize_Reg, 2, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nerror CRC_DISABLE!", strlen("\r\nerror CRC_DISABLE!"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);


	status = HAL_I2C_Master_Transmit(&hi2c1, MOTORUN_ADDR << 1 , CRC_DISABLE, 4, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nerror CRC_DISABLE!", strlen("\r\nerror CRC_DISABLE!"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

	status = HAL_I2C_Master_Transmit(&hi2c1, MOTORUN_ADDR << 1 , &MOTORN_VERSION, 1, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nerror MOTORN_VERSION!", strlen("\r\nerror MOTORN_VERSION!"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

	status = HAL_I2C_Master_Receive(&hi2c1, MOTORUN_ADDR << 1 , MOTORUN_RX_BUFFER, 4, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nerror MOTORUN_RX_BUFFER!", strlen("\r\nerror MOTORUN_RX_BUFFER!"), TIMEOUT_UART);
		return status;
	}

	uint16_t motorun_id = (uint16_t )((MOTORUN_RX_BUFFER[1] << 8) | (MOTORUN_RX_BUFFER[0]));
	uint16_t motorun_firmware = (uint16_t)((MOTORUN_RX_BUFFER[3] << 8 ) | (MOTORUN_RX_BUFFER[2]));

//    snprintf(uart_tx, sizeof(uart_tx),
//             "\r\nSTATUS: %d RX: %02X %02X %02X %02X Product ID: 0x%04X Firmware: 0x%04X",
//             status, MOTORUN_RX_BUFFER[0], MOTORUN_RX_BUFFER[1], MOTORUN_RX_BUFFER[2], MOTORUN_RX_BUFFER[3], motorun_id, motorun_firmware);
    snprintf(uart_tx, sizeof(uart_tx),
             "\r\nInitialization , I2C Status: %d , Product ID: 0x%04X Firmware: 0x%04X",
             status, motorun_id, motorun_firmware);

    HAL_UART_Transmit(&huart1, (uint8_t *)uart_tx, strlen(uart_tx), TIMEOUT_UART);

	return status;

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

  i2c_motoron();
  hal_i2c_motoron_initialization();

  float voltage = 0;
  int16_t adc_raw = 0;
  uint8_t i2c_rx_buff[2] = {0};
  uint8_t i2c_rx_buff_pot[2] = {0};

  i2c_rx_buff_pot[0] = i2c_rx_buff[0];
  i2c_rx_buff_pot[1] = i2c_rx_buff[1];

  static uint32_t last_speed = 32767;

  while (1)
  {

	  char uart_tx_i2c[80];

	  HAL_StatusTypeDef status;


	  for(uint8_t mux_ch = 0; mux_ch < 8 ; mux_ch ++)
	  {
		  status = i2c_mux_module(MULPLXR_CH_MASK[0] , CH_I2C_TX_BUFF[0] ,
		  					  	  	  	  	  ADC_CONFIG_SIZE , ADC_RX_SIZE, i2c_rx_buff , 0);
		  adc_raw = (int16_t )((i2c_rx_buff[0] << 8) | i2c_rx_buff[1]);
		  			  // to cancel the noise effect on the print:
		  if(adc_raw < 0)
		  {
			  adc_raw = 0;
		  }
		  voltage = adc_raw * 4.096 / 32768.0f;

		  sprintf(uart_tx_i2c , "\n\rMux[%d] , ADC%d: 0x%04x , voltage: %.4f v" , mux_ch , 0  , adc_raw, voltage);
		  HAL_UART_Transmit(&huart1, uart_tx_i2c, strlen(uart_tx_i2c), 100);

		  // To store the potentiometer value

			  i2c_rx_buff_pot[0] = i2c_rx_buff[0];
			  i2c_rx_buff_pot[1] = i2c_rx_buff[1];

//			  if(adc_raw != last_speed)				// Update only when the potentiometer changes
//			  {
				  i2c_motoron_motor_set(i2c_rx_buff_pot);
				  last_speed = adc_raw;
//			  }


		  for(uint8_t adc_ch = 0; adc_ch < 4 ; adc_ch++)
		  {
			  if(adc_ch != 0)
				  status = i2c_mux_module(MULPLXR_CH_MASK[mux_ch] , CH_I2C_TX_BUFF[adc_ch] ,
										  ADC_CONFIG_SIZE , ADC_RX_SIZE, i2c_rx_buff , adc_ch);
	//				snprintf(uart_tx_i2c , sizeof(uart_tx_i2c) , "\r\nPotentiometer111: 0x%02x 0x%02x " , i2c_rx_buff[0] ,i2c_rx_buff[1]  );
	//				HAL_UART_Transmit(&huart1, (uint8_t *) uart_tx_i2c, strlen(uart_tx_i2c), TIMEOUT_UART);


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
