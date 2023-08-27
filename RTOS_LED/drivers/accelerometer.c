/**
 * @file accelerometer.c
 *
 * @author Robert Merrill
 *
 * @brief accelerometer source file. Contains function to configure the Accelerometer
 * Contains to task to fetch the values of the xy and z registers for position and tilt of board.
 */
#include "accelerometer.h"
#include "i2c0.h"
#include <stdbool.h>
#include <stdint.h>
#include <MKL25Z4.h>
#include "taskdefs.h"
#include "led_pwm.h"



#define ACCEL_ADDR 0x3A
#define CTRL_REG1  0x2A
#define FIFO_SETUP_REG 0x09
#define F_MODE0    6
#define ACTIVE_BIT 0
#define F_READ_BIT 1
#define LNOISE_BIT 2
#define DR0        3
#define DR1        4
#define DR2        5
#define XREG       0x01 
#define YREG       0x03
#define ZREG       0x05     

struct i2c i2c_data;

/**
 * Configure Accelerometer
 */
void Configure_Accelerometer()
    {
    i2c0_write_byte(ACCEL_ADDR, CTRL_REG1, ((0<<DR2) | (0<<DR1) | (0<<DR0) | (1<<LNOISE_BIT) | (1<<F_READ_BIT) | (1<<ACTIVE_BIT)));
    while (!I2c0_Last_Transaction_Complete());
    }

/**
 * Task to read the xy and z register of the accelerometer and copy them into the i2c_data struct.
 * The command is set up read the 3 MSBs of the x, y and z register.
 * Task priority 1
 */
void ReadXYZTask(void *parameters)
    {
	while(1)
		{
		if (xSemaphoreTake(i2cDataMutex, portMAX_DELAY) == TRUE)
			{
			i2c0_read_bytes(ACCEL_ADDR, XREG, 3, &i2c_data.xdata);
			xSemaphoreGive(i2cDataMutex);
			}

		vTaskDelay(pdMS_TO_TICKS(5));
		}
    }




