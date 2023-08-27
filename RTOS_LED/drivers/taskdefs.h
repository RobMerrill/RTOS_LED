/**
 * @file taskdefs.h
 *
 * @author Robert Merrill
 *
 * @brief Contains task prototypes
 */

#ifndef TASKDEFS_H
#define TASKDEFS_H

#include "FreeRTOS.h"
#include "task.h"

void ModifyDutyCycleTask(void *parameters);
void ReadXYZTask(void *parameters);
void CommandTask(void *paramters);
void ReadUartTask(void *paramters);

#endif
