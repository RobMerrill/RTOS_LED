/**
 * @file i2c0.h
 *
 * @author Robert Merrill
 *
 * @brief i2c0 header file. Contains function prototypes to configure and use the i2c interface.
 */

#ifndef I2C0_H
#define I2C0_H
#include <stdbool.h>
#include <stdint.h>


_Bool i2c0_write_byte( uint8_t device, uint8_t reg, uint8_t data);
_Bool i2c0_read_bytes(uint8_t device, uint8_t reg, uint8_t count, uint8_t *data);
void Configure_I2c0();
_Bool I2c0_Last_Transaction_Complete();
_Bool I2c0_Last_Transaction_Succeeded();

struct i2c{
    uint8_t xdata;
    uint8_t ydata;
    uint8_t zdata;
    _Bool xmod : 1, ymod : 1, zmod : 1;
};

extern struct i2c i2c_data;


#endif
