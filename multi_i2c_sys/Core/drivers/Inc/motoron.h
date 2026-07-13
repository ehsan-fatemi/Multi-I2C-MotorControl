#ifndef MOTORON_H
#define MOTORON_H

#include <inttypes.h>
#include "main.h"

#define MOTORUN_ADDR 		 0x10U
typedef struct
{
	int16_t speed;
	int16_t adc_raw;
	uint16_t voltage_mv;

}PotResult;


HAL_StatusTypeDef i2c_motoron_motor_set(I2C_HandleTypeDef * , UART_HandleTypeDef *  , uint8_t * , int16_t );
HAL_StatusTypeDef hal_i2c_motoron_initialization(I2C_HandleTypeDef * , UART_HandleTypeDef * );
HAL_StatusTypeDef i2c_motoron(I2C_HandleTypeDef * , UART_HandleTypeDef *);
PotResult potentiometer_to_speed(uint8_t * );



#endif
