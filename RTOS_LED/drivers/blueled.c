/**
 * @file blueled.c
 *
 * @author Robert Merrill
 *
 * @brief Blue LED source file. Contains functions to turn on and off the blue led
 */

#include <MKL25Z4.h>
#include "blueled.h"

#define BLUE_LED_LOC 1

void configure_blue_led()
{
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[BLUE_LED_LOC] =
	    (
	    PORTD->PCR[BLUE_LED_LOC] & ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK))
	    | PORT_PCR_IRQC_MASK | PORT_PCR_MUX(1);
	PTD->PDDR |= (1<<BLUE_LED_LOC);
	turn_off_blue_led();
}
void turn_on_blue_led()
{
	PTD->PCOR |= (1<<BLUE_LED_LOC);
}
void turn_off_blue_led()
{
	PTD->PSOR |= (1<<BLUE_LED_LOC);
}
void toggle_blue_led()
{
	PTD->PTOR |= (1<<BLUE_LED_LOC);
}
