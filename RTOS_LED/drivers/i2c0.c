/**
 * @file i2c0.c
 *
 * @author Robert Merrill
 *
 * @brief I2C0 source file. contains function definitions, data structures and the ISR for the I2C0 interface.
 */

#include <stdbool.h>
#include <stdint.h>
#include <MKL25Z4.h>
#include "i2c0.h"

#ifndef I2C0_IRQ_PRIORITY
#define I2C0_IRQ_PRIORITY 3
#endif
static void i2c0_do_start(uint8_t);
static void i2c0_do_stop();
static void i2c0_do_repeat_start(uint8_t);
static void i2c0_put_byte(uint8_t);
static uint8_t i2c0_get_byte();
static void i2c0_nak_after_byte_recieved();
static _Bool i2c0_ack_was_recieved();
static void i2c0_ack_after_byte_recieved();
static _Bool i2c0_is_busy();
_Bool I2c0_Last_Transaction_Complete();
_Bool I2c0_Last_Transaction_Succeeded();

static volatile struct{
    uint8_t device;
    uint8_t reg;
    uint8_t data;
    uint8_t count;
    uint8_t *pdata;
    _Bool busy, error, done, write;
}isr_data;

_Bool I2c0_Last_Transaction_Complete()
    {
    return isr_data.done;
    }

_Bool I2c0_Last_Transaction_Succeeded()
    {
    return !isr_data.error && isr_data.done;
    }
_Bool i2c0_write_byte( uint8_t device, uint8_t reg, uint8_t data)
    {
    if (i2c0_is_busy() || isr_data.busy)
        {
        return false;
        }
    isr_data.device = device;
    isr_data.reg = reg;
    isr_data.data = data;
    isr_data.busy = true;
    isr_data.error = false;
    isr_data.done = false;
    isr_data.write = true;
    i2c0_do_start(device | 0x00);
    return true;
    }

_Bool i2c0_read_bytes(uint8_t device, uint8_t reg, uint8_t count, uint8_t *data)
    {
    if (i2c0_is_busy() || isr_data.busy)
        {
        return false;
        }
    isr_data.device = device;
    isr_data.reg = reg;
    isr_data.pdata = data;
    isr_data.count = count;
    isr_data.busy = true;
    isr_data.error = false;
    isr_data.done = false;
    isr_data.write = false;
    i2c0_do_start(device | 0x00);
    return true;
    }

void Configure_I2c0()
    {
    SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[24] = (PORTE->PCR[24] &! PORT_PCR_MUX_MASK) | PORT_PCR_MUX(5);
    PORTE->PCR[25] = (PORTE->PCR[25] &! PORT_PCR_MUX_MASK) | PORT_PCR_MUX(5);
    I2C0->F = I2C_F_ICR(0x12) | I2C_F_MULT(0);
    I2C0->C1 |= I2C_C1_IICEN_MASK | I2C_C1_IICIE_MASK;
    I2C0->S |= I2C_S_IICIF_MASK;
    NVIC_SetPriority(I2C0_IRQn, I2C0_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(I2C0_IRQn);
    NVIC_EnableIRQ(I2C0_IRQn);
    }

static void i2c0_do_start(uint8_t device_addr)
    {
    I2C0->C1 |= (I2C_C1_TX_MASK | I2C_C1_MST_MASK);
    I2C0->D = device_addr;
    }

static void i2c0_do_stop()
    {
    I2C0->C1 &= ~I2C_C1_MST_MASK;
    }

static void i2c0_do_repeat_start(uint8_t device_addr)
    {
        I2C0->C1 |= (I2C_C1_TX_MASK | I2C_C1_RSTA_MASK);
        I2C0->D = device_addr;
    }

static void i2c0_put_byte(uint8_t data)
    {
    I2C0->D = data;
    }

static uint8_t i2c0_get_byte()
    {
    I2C0->C1 &= ~I2C_C1_TX_MASK;
    return I2C0->D;
    }

static void i2c0_nak_after_byte_recieved()
    {
    I2C0->C1 |= I2C_C1_TXAK_MASK;
    }

static _Bool i2c0_ack_was_recieved()
    {
        return !(I2C0->S & I2C_S_RXAK_MASK);
    }

static void i2c0_ack_after_byte_recieved()
    {
    I2C0->C1 &= ~I2C_C1_TXAK_MASK;
    }

static _Bool i2c0_is_busy()
    {
    return I2C0->S & I2C_S_BUSY_MASK;
    }
void I2C0_IRQHandler()
    {
    static enum {REG, WRITE1, WRITE2, READ1, READ2} state=REG;
    static uint8_t i;
    I2C0->S |= I2C_S_IICIF_MASK;
    switch (state)
    {
        case REG:
            if (!i2c0_ack_was_recieved())
                {
                goto IRQ_ERROR;
                }
            i2c0_put_byte(isr_data.reg);
            if (isr_data.write)
                {
                state = WRITE1;
                }
            else   
                {
                state = READ1;
                }
            break;
        case WRITE1:
            if (!i2c0_ack_was_recieved())
                {
                goto IRQ_ERROR;
                }
            i2c0_put_byte(isr_data.data);
            state = WRITE2;
            break;
        case WRITE2:
            if (!i2c0_ack_was_recieved())
                {
                goto IRQ_ERROR;
                }
            i2c0_do_stop();
            isr_data.busy = false;
            isr_data.error = false;
            isr_data.done = true;
            state = REG;
            break;
        case READ1:
            if (!i2c0_ack_was_recieved())
                {
                goto IRQ_ERROR;
                }
            i2c0_do_repeat_start(isr_data.device | 0x01);
            state = READ2;
            i = isr_data.count + 1;
            i2c0_ack_after_byte_recieved();
            break;
        case READ2:
            if ((i == isr_data.count+1) && (!i2c0_ack_was_recieved()))
                {
                goto IRQ_ERROR;
                }
            if (i--)
                {
                if (i==0)
                    {
                    i2c0_do_stop();
                    }
                if (i==1)
                    {
                    i2c0_nak_after_byte_recieved();
                    }
                if (i == isr_data.count)
                    {
                    i2c0_get_byte();
                    }
                else
                    {
                    *isr_data.pdata++=i2c0_get_byte();
                    }
                }
            if (i == 0)
                {
                isr_data.done = true;
                isr_data.busy = false;
                isr_data.error = false;
                state = REG;
                }
            break;
        
        default:
            break;
    }
    return;
IRQ_ERROR:
    i2c0_do_stop();
    isr_data.busy = false;
    isr_data.error = true;
    isr_data.done = true;
    state = REG;
    return;
    }
