/**
 * \ingroup wrappers
 * @file FreeRTOSMutex.cpp
 *
 * C++ wrapper for FreeRTOS mutex implementation
 *
 * \date 17. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 */

#include "FreeRTOSMutex.h"

FreeRTOSMutex::FreeRTOSMutex() :
		sem(xSemaphoreCreateMutex()) {
}

void FreeRTOSMutex::lock() {
	xSemaphoreTake(sem, portMAX_DELAY);
}

void FreeRTOSMutex::unlock() {
	xSemaphoreGive(sem);
}
