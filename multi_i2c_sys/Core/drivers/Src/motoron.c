#include "motoron.h"
#include "app_config.h"

#define POT_ADC_MIN  0
#define POT_ADC_MAX  26400
#define MOTOR_MAX_SPEED 800


HAL_StatusTypeDef i2c_motoron_motor_set(I2C_HandleTypeDef *hi2c , UART_HandleTypeDef *huart , uint8_t * i2c_rx_buff_pot , int16_t speed)
{
	HAL_StatusTypeDef status;
	char uart_tx[100];

	uint8_t motoron_set_speed[4] ;//= {0xD1 , 0x01 , 0x00 , 0x40};

	snprintf(uart_tx , sizeof(uart_tx) , "\r\nPotentiometer: 0x%02x 0x%02x" , i2c_rx_buff_pot[0] ,i2c_rx_buff_pot[1]);
	HAL_UART_Transmit(huart, (uint8_t *) uart_tx, strlen(uart_tx), TIMEOUT_UART);



	uint8_t motor = 0x01;
	motoron_set_speed[0] = 0xD1;
	motoron_set_speed[1] = motor & 0x7F;
	motoron_set_speed[2] = speed & 0x7F;
	motoron_set_speed[3] = (speed >> 7) & 0x7F;

	snprintf(uart_tx , sizeof(uart_tx) , "\r\nmotoron_set_speed: 0x%04x 0x%04x 0x%04x 0x%04x speed:%d " , motoron_set_speed[0] ,motoron_set_speed[1] , motoron_set_speed[2] , motoron_set_speed[3] , speed);
	HAL_UART_Transmit(huart, (uint8_t *) uart_tx, strlen(uart_tx), TIMEOUT_UART);


	status = HAL_I2C_Master_Transmit(hi2c, MOTORUN_ADDR << 1 , motoron_set_speed, 4 , TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		uint32_t err = HAL_I2C_GetError(hi2c);

		snprintf(uart_tx, sizeof(uart_tx),
		         "\r\nI2C failed. status=%d error=0x%08lX state=%d",
		         status, err, hi2c->State);

		HAL_UART_Transmit(huart, (uint8_t *)uart_tx, strlen(uart_tx), TIMEOUT_UART);


		return status;
	}
	HAL_Delay(10);

	return status;
}

HAL_StatusTypeDef hal_i2c_motoron_initialization(I2C_HandleTypeDef *hi2c , UART_HandleTypeDef *huart)
{
	HAL_StatusTypeDef status;
	char uart_tx[100];
	uint8_t motoron_clear_motor_fault = 0xA6;

	status = HAL_I2C_Master_Transmit(hi2c, MOTORUN_ADDR << 1 , &motoron_clear_motor_fault, 1, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(huart, (uint8_t *)"\r\nerror motoron_clear_motor_fault", strlen("\r\nerror motoron_clear_motor_fault"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

	uint8_t motoron_clear_latched_status_flags[] = {0xA9 , 0x00 , 0x04};
	status = HAL_I2C_Master_Transmit(hi2c, MOTORUN_ADDR << 1 , motoron_clear_latched_status_flags, 3, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(huart, (uint8_t *)"\r\nerror motoron_clear_latched_status_flags", strlen("\r\nerror motoron_clear_latched_status_flags"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

	return status;

}

HAL_StatusTypeDef i2c_motoron(I2C_HandleTypeDef *hi2c , UART_HandleTypeDef *huart)
{
	char uart_tx[100];

	static const uint8_t Reinitialize_Reg[] = {0x96 , 0x74};
	static const uint8_t CRC_DISABLE[] = {0x8B , 0x04 , 0x7B , 0x43};
	static const uint8_t MOTORN_VERSION 	= 0x87;
	uint8_t MOTORUN_RX_BUFFER[4] = {0};

	HAL_StatusTypeDef status;
	status = HAL_I2C_Master_Transmit(hi2c, MOTORUN_ADDR << 1 , Reinitialize_Reg, 2, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(huart, (uint8_t *)"\r\nerror CRC_DISABLE!", strlen("\r\nerror CRC_DISABLE!"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);


	status = HAL_I2C_Master_Transmit(hi2c, MOTORUN_ADDR << 1 , CRC_DISABLE, 4, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(huart, (uint8_t *)"\r\nerror CRC_DISABLE!", strlen("\r\nerror CRC_DISABLE!"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

	status = HAL_I2C_Master_Transmit(hi2c, MOTORUN_ADDR << 1 , &MOTORN_VERSION, 1, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(huart, (uint8_t *)"\r\nerror MOTORN_VERSION!", strlen("\r\nerror MOTORN_VERSION!"), TIMEOUT_UART);
		return status;
	}
	HAL_Delay(10);

	status = HAL_I2C_Master_Receive(hi2c, MOTORUN_ADDR << 1 , MOTORUN_RX_BUFFER, 4, TIMEOUT_I2C);
	if(status != HAL_OK)
	{
		HAL_UART_Transmit(huart, (uint8_t *)"\r\nerror MOTORUN_RX_BUFFER!", strlen("\r\nerror MOTORUN_RX_BUFFER!"), TIMEOUT_UART);
		return status;
	}

	uint16_t motorun_id = (uint16_t )((MOTORUN_RX_BUFFER[1] << 8) | (MOTORUN_RX_BUFFER[0]));
	uint16_t motorun_firmware = (uint16_t)((MOTORUN_RX_BUFFER[3] << 8 ) | (MOTORUN_RX_BUFFER[2]));


    snprintf(uart_tx, sizeof(uart_tx),
             "\r\nInitialization , I2C Status: %d , Product ID: 0x%04X Firmware: 0x%04X",
             status, motorun_id, motorun_firmware);

    HAL_UART_Transmit(huart, (uint8_t *)uart_tx, strlen(uart_tx), TIMEOUT_UART);

	return status;

}

PotResult potentiometer_to_speed(uint8_t * i2c_rx_buff)
{
	PotResult result;

	int16_t adc_raw = 0;
	  adc_raw = (int16_t )((i2c_rx_buff[0] << 8) | i2c_rx_buff[1]);

	  int32_t adc_clamped = adc_raw;
	  if (adc_clamped < POT_ADC_MIN)
		{
		    adc_clamped = POT_ADC_MIN;
		}

		if (adc_clamped > POT_ADC_MAX)
		{
		    adc_clamped = POT_ADC_MAX;
		}

	  int16_t speed = (int16_t)(((adc_clamped - POT_ADC_MIN) * 1600L) / (POT_ADC_MAX - POT_ADC_MIN) - 800);
	  			  // to cancel the noise effect on the print:
	  if(adc_raw < 0)
	  {
		  adc_raw = 0;
	  }

	  static int16_t last_commanded_speed = 0;

		  if (abs(speed - last_commanded_speed) >= 15)
		  {
			  last_commanded_speed = speed;
			}
		  result.voltage_mv = (uint16_t)((uint32_t) adc_raw * 4096U / 32768U);
		  result.speed = last_commanded_speed;
		  result.adc_raw = adc_raw;

		  return result;

}
