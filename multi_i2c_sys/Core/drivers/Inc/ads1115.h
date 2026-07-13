#ifndef ADS1115_H
#define ADS1115_H

#include <inttypes.h>
#include "main.h"

#define ADC_CONFIG_SIZE 			3U
#define ADC_RX_SIZE    				2U
#define ADS1115_I2C_ADDR 			0x48U

static const uint8_t CONV_REG     = 0x00U;




HAL_StatusTypeDef i2c_adc_module(I2C_HandleTypeDef * , uint8_t , const uint8_t*,
								uint8_t  ,  uint8_t * ,
								uint8_t );

#endif
