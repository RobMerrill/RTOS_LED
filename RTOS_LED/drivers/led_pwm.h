/**
 * @file led_pwm.h
 *
 * @author Robert Merrill
 *
 * @brief Led_pwm header file. Contain function prototype to configure the TPM of the three LEDs.

 */
#ifndef LED_PWM_H
#define LED_PWM_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define TRUE 1

extern SemaphoreHandle_t i2cDataMutex;
extern SemaphoreHandle_t charBufferMutex;
void Configure_Led_Pwm();
void InitializeI2CDataMutex(void);

#endif
