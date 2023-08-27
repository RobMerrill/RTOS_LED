/**
 * @file ser_commands.c
 *
 * @author Robert Merrill
 *
 * @brief serial commands source file. Contains command definitions and command execution task.
 */

#include <stdio.h>
#include "ser_commands.h"
#include "uart0_txirq.h"
#include "taskdefs.h"
#include "uclib.h"
#include "i2c0.h"
#include "led_pwm.h"

uint8_t value;
char ok_response[] = "OK\r\n";

struct commands {
	uint8_t buffer[CMDBUF];
	_Bool command_flag;
	uint8_t buf_index;
	uint8_t cr_index;
}com;

static void execute_m_command(void);
static void execute_p_command(void);
void bool_to_string(_Bool value);
static void clear_buffer(void);

void execute_m_command(void)
	{
	switch (com.buffer[1])
		{
		case 'X':
		case 'x':
			if (com.buffer[3] == '1')
				{
				i2c_data.xmod = true;
				}
			else
				{
				i2c_data.xmod = false;
				}
			print_string(ok_response, 4);
			break;
		case 'Y':
		case 'y':
			if (com.buffer[3])
				{
				i2c_data.ymod = true;
				}
			else
				{
				i2c_data.ymod = false;
				}
			print_string(ok_response, 4);
			break;
		case 'Z':
		case 'z':
			if (com.buffer[3])
				{
				i2c_data.zmod = true;
				}
			else
				{
				i2c_data.zmod = false;
				}
			print_string(ok_response, 4);
		case 'S':
		case 's':
			print_string("XMOD: ", 6);
			bool_to_string(i2c_data.xmod);
			print_string("\r\n", 3);
			print_string("YMOD: ", 6);
			bool_to_string(i2c_data.ymod);
			print_string("\r\n", 3);
			print_string("ZMOD: ", 6);
			bool_to_string(i2c_data.zmod);
			print_string("\r\n", 3);
			break;
		}
	}

void execute_p_command(void)
	{
	switch (com.buffer[1])
		{
		case 'X':
		case 'x':
			print_string("X Position: ", 12);
			print_hex8(i2c_data.xdata);
			print_string("\r\n", 3);
			break;
		case 'Y':
		case 'y':
			print_string("Y Position: ", 12);
			print_hex8(i2c_data.ydata);
			print_string("\r\n", 3);
			break;
		case 'Z':
		case 'z':
			print_string("Z Position: ", 12);
			print_hex8(i2c_data.zdata);
			print_string("\r\n", 3);
			break;
		default:
			print_string("?\r\n", 3);
			break;
		}
	}

void bool_to_string(_Bool value)
	{
	if (value)
		{
		print_string("True", 4);
		}
	else
		{
		print_string("False", 5);
		}
	}

void clear_buffer(void)
	{
	uint8_t i;
	for (i = 0; i < CMDBUF; i++)
		{
		com.buffer[i] = 0;
		}
	com.buf_index = 0;
	}

void CommandTask(void *paramters)
	{
	while (1)
		{
		if (xSemaphoreTake(i2cDataMutex, portMAX_DELAY) == TRUE)
			{
			if (com.command_flag)
				{
				switch (com.buffer[0])
					{
					case 'M':
					case 'm':
						execute_m_command();
						break;
					case 'P':
					case 'p':
						execute_p_command();
						break;
					}
				clear_buffer();
				com.command_flag = false;
				}
			xSemaphoreGive(i2cDataMutex);
			}
		vTaskDelay(pdMS_TO_TICKS(10));
		}
	}

void ReadUartTask(void *paramters)
	{
	while (1)
		{
		if (Recieve_Data_From_Uart0(&value))
			{
			if (value == CR)
				{
				print_string("\n", 1);
				com.command_flag = true;
				com.buffer[com.buf_index] = value;
				com.cr_index = com.buf_index;
				}
			else
				{
				com.buffer[com.buf_index] = value;
				com.buf_index++;
				}
			}
		vTaskDelay(pdMS_TO_TICKS(10));
		}
	}



