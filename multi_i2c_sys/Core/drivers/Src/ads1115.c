#include "ads1115.h"
#include "app_config.h"

HAL_StatusTypeDef i2c_adc_module(I2C_HandleTypeDef * hi2c, uint8_t adc_channel , const uint8_t i2c_tx_buff[] , uint8_t tx_arr_size  ,  uint8_t * i2c_rx_buff , uint8_t rx_arr_size)
  {


	  HAL_StatusTypeDef status;

	  status = HAL_I2C_Master_Transmit(hi2c, ADS1115_I2C_ADDR << 1 , i2c_tx_buff, tx_arr_size , TIMEOUT_I2C);

	  if(status != HAL_OK)
	  {
		  return status;
	  }

	  HAL_Delay(10);

	  // To read from the sensor
	  status = HAL_I2C_Master_Transmit(hi2c, ADS1115_I2C_ADDR << 1, &CONV_REG , 1, TIMEOUT_I2C);

	  if(status != HAL_OK)
	  	  {
	  		  return status;
	  	  }

	  HAL_Delay(10);

	  status = HAL_I2C_Master_Receive(hi2c, ADS1115_I2C_ADDR << 1 , i2c_rx_buff , rx_arr_size, TIMEOUT_I2C);

	  if(status != HAL_OK)
	  	  {
	  		  return status;
	  	  }



	  return HAL_OK;

  }
