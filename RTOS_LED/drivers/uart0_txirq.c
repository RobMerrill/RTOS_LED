/* Source file for irq base tx UART0 Driver*/
/*Robert Merrill*/
#include <MKL25Z4.h>
#include "uart0_txirq.h"
#include <stdbool.h>
#include <stdint.h>

#define FBAUDHIGH 0
#define FBAUDLOW  156 //115.2KiB

#ifndef UART0_PRIORITY
#define UART0_PRIORITY 3
#endif

#ifndef TXBUFSIZE
#define TXBUFSIZE 256
#endif

#define IS_POWER_OF_2(N)  (((N)&((N)-1))) == 0
#if !IS_POWER_OF_2(TXBUFSIZE)
#error TXBUFSIZE must be a power of 2
#endif

static volatile uint32_t head, tail;
static volatile uint8_t tx_buf[TXBUFSIZE];

#define next_head() ( (head+1) & (TXBUFSIZE-1) )
#define next_tail() ( (tail+1) & (TXBUFSIZE-1) )

static _Bool txbuf_empty()
    {
    return head == tail;
    }

static _Bool txbuf_full()
    {
    return next_head() == tail;
    }
static void txbuf_enqueue (uint8_t data)
    {
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    tx_buf[head] = data;
    head = next_head();
    __set_PRIMASK(mask);
    }

static uint8_t txbuf_dequeue ()
    {
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    uint8_t retval = tx_buf[tail];
    tail = next_tail();
    __set_PRIMASK(mask);
    return retval;
    }

void Configure_Uart0_Txirq()
    {
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[1] = PORT_PCR_MUX(2);
    PORTA->PCR[2] = PORT_PCR_MUX(2);
    SIM->SOPT2 |= (SIM_SOPT2_UART0SRC(1));
    UART0->BDH = FBAUDHIGH;
    UART0->BDL = FBAUDLOW;
    UART0->C1 = 0;
    UART0->C2 = UART0_C2_TE_MASK | UART0_C2_RE_MASK;
    NVIC_SetPriority(UART0_IRQn, UART0_PRIORITY);
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);
    }

_Bool Send_Data_To_Uart0_Irq( uint8_t data)
    {
    if (!txbuf_full())
        {
        txbuf_enqueue(data);
        UART0->C2 |= UART0_C2_TIE_MASK;
        return true;
        }
    return false;
    }

_Bool Recieve_Data_From_Uart0( uint8_t *data)
    {
    if (UART0->S1 & UART0_S1_RDRF_MASK)
        {
        *data = UART0->D;
        return true;
        }
    return false;
    }
void UART0_IRQHandler()
    {
    if (!txbuf_empty())
        {
        UART0->D = txbuf_dequeue();
        return;
        }
    UART0->C2 ^= UART0_C2_TIE_MASK;
    }
