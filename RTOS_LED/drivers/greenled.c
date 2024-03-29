/**
 * @file greenled.c
 *
 * @author Robert Merrill
 *
 * @brief Green LED source file. Contains functions to turn on and off the green led
 */
#include <MKL25Z4.h>
#include "greenled.h"


#define GREEN_LED_LOC 19

void configure_green_led()
{
        SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
        PORTB->PCR[GREEN_LED_LOC] =
           (PORTB->PCR[GREEN_LED_LOC] & ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK)) 
           | PORT_PCR_IRQC_MASK | PORT_PCR_MUX(1);
        PTB->PDDR |= (1<<GREEN_LED_LOC);
        turn_off_green_led();
}
void turn_on_green_led()
{
        PTB->PCOR |= (1<<GREEN_LED_LOC);
}
void turn_off_green_led()
{
        PTB->PSOR |= (1<<GREEN_LED_LOC);
}
void toggle_green_led()
{
	PTB->PTOR |= (1<<GREEN_LED_LOC);
}

