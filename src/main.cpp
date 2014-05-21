/*
 * main.cpp
 *
 *  Created on: 15. 9. 2013
 *      Author: Tomas Bures <bures@d3s.mff.cuni.cz>
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "main.h"
#include "UART.h"
#include "test/TestComponent.h"
#include "test/PortableThermometer.h"

#include <cstdio>
#include <sstream>
#include <string>
#include <signal.h>

using namespace std;

// Initialize system console
#include "Console.h"

// Initialize delay timer
Timer::Properties tim6Props {
TIM6, RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM6, TIM6_DAC_IRQn };
Timer delayTimer(tim6Props);

/** System startup function */
int main(void) {
	delayTimer.setPriority(1, 1);
	delayTimer.init();
	Console::init();
	Console::log("\n\n\n\n\n\n\n\n\n\n");
	Console::log("# # # # # # # # # # # # # # # # # # # #");
	Console::log(" # # # # # # # # # # # # # # # # # # #");
	Console::log("# # # # # # # # # # # # # # # # # # # #");
	Console::log("\n>>> SYSTEM INIT <<<");

	Console::log(">>> Waiting 3s for debugger to stop me...");
	delayTimer.mDelay(3000);
	Console::log(">>> Starting system");

	Console::log(">>> About to construct system object");
	System *system = new System();

	Console::log(">>> About to construct test component");
	new TestComponent(*system);

	Console::log(">>> About to temperature component");
		new PortableThermometer(*system, 1);

	Console::log(">>> Running scheduler");

	// Start the scheduler.
	vTaskStartScheduler();

	// This should not be reached
	Console::log(">>> End reached - THIS SHOULD NOT HAPPEN !!!!");
	while(1) {
	}
}

// FreeRTOS System error handlers
extern "C" {
	void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
		Console::log("STACK OVERFLOW!!");
		while(1) {
		}
	}

	void vApplicationMallocFailedHook(void) {
		Console::log("MALLOC FAILED!!!");
		while(1) {
		}
	}
}
