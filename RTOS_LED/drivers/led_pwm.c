/**
 * @file led_pwm.c
 *
 * @author Robert Merrill
 *
 * @brief Led_pwm source file. Contain function to configure the TPM of the three LEDs.
 * Contains the task that modifies the duty cycle of the the individual TPMs
 */

#include <stdint.h>
#include <MKL25Z4.h>
#include "i2c0.h"
#include "taskdefs.h"
#include "led_pwm.h"


#define RED_LED_LOC 18
#define GREEN_LED_LOC 19
#define BLUE_LED_LOC 1
#define RED_LED_CH   0
#define GREEN_LED_CH 1
#define BLUE_LED_CH  1
#define MAX_TOP      0x00FF

SemaphoreHandle_t i2cDataMutex;

void Configure_Led_Pwm()
    {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[BLUE_LED_LOC] = PORT_PCR_MUX(4);
    PORTB->PCR[RED_LED_LOC] |= PORT_PCR_MUX(3);
    PORTB->PCR[GREEN_LED_LOC] =PORT_PCR_MUX(3);
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    TPM2->SC |= (TPM_SC_CPWMS(0) | TPM_SC_CMOD(1));
    TPM2->SC |= TPM_SC_PS(7);
    TPM0->SC |= (TPM_SC_CPWMS(0) | TPM_SC_CMOD(1));
    TPM0->SC |= TPM_SC_PS(7);
    TPM2->CONTROLS[RED_LED_CH].CnSC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_ELSA(0) |
                   TPM_CnSC_MSB(1)  | TPM_CnSC_MSA(0));
    TPM2->CONTROLS[GREEN_LED_CH].CnSC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_ELSA(0) |
                   TPM_CnSC_MSB(1)  | TPM_CnSC_MSA(0));
    TPM0->CONTROLS[BLUE_LED_CH].CnSC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_ELSA(0) |
                   TPM_CnSC_MSB(1)  | TPM_CnSC_MSA(0));
    TPM2->CNT = 0;
    TPM2->MOD = MAX_TOP;
    TPM2->CONTROLS[RED_LED_CH].CnV = 0;
    TPM0->CNT = 0;
    TPM0->MOD = MAX_TOP;
    TPM2->CONTROLS[GREEN_LED_CH].CnV = 0;
    TPM0->CONTROLS[BLUE_LED_CH].CnV = 0;
    }

void modify_green_duty_cycle(void)
	{
	if (i2c_data.xmod)
		{
		TPM2->CONTROLS[GREEN_LED_CH].CnV = i2c_data.xdata;
		}
	}

void modify_red_duty_cycle(void)
	{
	if (i2c_data.ymod)
		{
		TPM2->CONTROLS[RED_LED_CH].CnV = i2c_data.ydata;
		}
	}

void modify_blue_duty_cycle(void)
	{
	if (i2c_data.zmod)
		{
		TPM0->CONTROLS[BLUE_LED_CH].CnV = i2c_data.zdata;
		}
	}

void ModifyDutyCycleTask(void *parameters)
	{
	while(1)
		{
		if (xSemaphoreTake(i2cDataMutex, portMAX_DELAY) == TRUE)
			{
			modify_green_duty_cycle();
			modify_red_duty_cycle();
			modify_blue_duty_cycle();
			xSemaphoreGive(i2cDataMutex);
			}
		vTaskDelay(pdMS_TO_TICKS(5));
		}
	}

void InitializeI2CDataMutex(void)
	{
	i2cDataMutex = xSemaphoreCreateMutex();
	}
