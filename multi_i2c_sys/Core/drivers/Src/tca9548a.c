
#include "tca9548a.h"
#include "app_config.h"

const uint8_t MULPLXR_CH_MASK[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};


HAL_StatusTypeDef i2c_mux_module(I2C_HandleTypeDef * hi2c, UART_HandleTypeDef *huart ,uint8_t mulplxr_channel , const uint8_t * i2c_tx_buff ,
								uint8_t tx_arr_size , uint8_t rx_arr_size,
								uint8_t * i2c_rx_buff , uint8_t adc_channel
								){





		HAL_StatusTypeDef status;

		status = HAL_I2C_Master_Transmit(hi2c, MULPLXR_ADDR << 1 , &mulplxr_channel , 1 , TIMEOUT_I2C);
		if(status != HAL_OK)
		{
			HAL_UART_Transmit(huart, (uint8_t *)"\r\nerrMULPLXR_ADDR", strlen("\r\nerrMULPLXR_ADDR"), TIMEOUT_UART);
			return status;
		}
		HAL_Delay(10);

		status = i2c_adc_module(hi2c ,adc_channel , i2c_tx_buff , tx_arr_size ,  i2c_rx_buff , rx_arr_size);
		if(status != HAL_OK)
		{
			return status;
		}



	return HAL_OK;
}
