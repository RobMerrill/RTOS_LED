/**
 * @file project.c
 *
 * @author Robert Merrill
 *
 * @brief Main source file. Contains initializations for interfaces, task creation and starts the scheduler.
 */

#include "accelerometer.h"
#include "i2c0.h"
#include "led_pwm.h"
#include "taskdefs.h"
#include "uart0_txirq.h"
#include "clock.h"

void _disable_irq(void) { asm volatile ("cpsid i" : : : "memory");}
void _enable_irq(void) { asm volatile ("cpsie i" : : : "memory");}


int main()
    {
    _disable_irq();
    SystemClockInit();
    Configure_I2c0();
    Configure_Led_Pwm();
    Configure_Uart0_Txirq();
    _enable_irq();
    Configure_Accelerometer();
    InitializeI2CDataMutex();
    xTaskCreate(ReadXYZTask, "Read XYZ Reg Accelerometer", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(ModifyDutyCycleTask, "Modify TPM Duty Cycles", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(ReadUartTask, "Read Uart0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(CommandTask, "Execute Serial Commands", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, NULL);

    vTaskStartScheduler();

    while(1)
    	{
    	__asm("NOP");
    	}
    return 0;
    }


