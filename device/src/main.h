// Prevent recursive inclusion
#ifndef __MAIN_H
#define __MAIN_H
#define __main_h__
#ifdef __cplusplus
    extern "C" {
#endif


// Vendor Drivers and HAL
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "vl53l0x_proximity.h"


// Standard
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


// Constants
enum {
  BP_NOT_PUSHED = 0, BP_SINGLE_PUSH, BP_MULTIPLE_PUSH
};

// Functions
voif Error_Handler( void );
uint8_t Button_WaitForPush( uint32_t timeout );
void Led_On( void );
void Led_Off( void );

#ifdef __cplusplus
    }
#endif

#endif
