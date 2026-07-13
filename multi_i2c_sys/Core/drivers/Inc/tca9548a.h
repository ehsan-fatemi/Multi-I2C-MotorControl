#ifndef TCA9548A_H
#define  TCA9548A_H

#include <inttypes.h>
#include "main.h"


#define MULPLXR_ADDR 		 0x70U


static const uint8_t CH_I2C_TX_BUFF[][3] = {
		{0x01, 0xC2, 0x80},
		{0x01, 0xD2, 0x80},
		{0x01, 0xE2, 0x80},
		{0x01, 0xF2, 0x80}
};

extern const uint8_t MULPLXR_CH_MASK[];

HAL_StatusTypeDef i2c_mux_module(I2C_HandleTypeDef * ,  UART_HandleTypeDef * , uint8_t  , const uint8_t * ,
								uint8_t  , uint8_t ,
								uint8_t *  , uint8_t );


#endif
