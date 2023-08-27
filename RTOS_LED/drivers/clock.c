/**
 * @file clock.c
 *
 * @author Robert Merrill
 *
 * @brief clock source file. Contains Init function for the system clock
 */
#include <MKL25Z4.h>
#include "clock.h"


void SystemClockInit(void)
	{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x01) | SIM_CLKDIV1_OUTDIV4(0x01);
	SIM->SOPT1 &= ~SIM_SOPT1_OSC32KSEL(0x03);
	SIM->SOPT2 = (SIM->SOPT2 &~ (SIM_SOPT2_TPMSRC_MASK)) | SIM_SOPT2_TPMSRC(0x01)|SIM_SOPT2_PLLFLLSEL_MASK;
	PORTA->PCR[18] &= ~((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
	PORTA->PCR[19] &= ~((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
	OSC0->CR = 0;
	MCG->C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
	MCG->C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03));
	MCG->C4 &= ~((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
	MCG->C5 = MCG_C5_PRDIV0(0x01);
	MCG->C6 = 0;
	while((MCG->S & MCG_S_IREFST_MASK) != 0);
	while((MCG->S & MCG_S_CLKST_MASK) != 8);
	MCG->C6 = MCG_C6_PLLS_MASK;
	while((MCG->S & MCG_S_LOCK0_MASK) == 0);
	MCG->C1 = MCG_C1_FRDIV(0x03);
	while((MCG->S & MCG_S_CLKST_MASK) != 0x0CU);
	}
