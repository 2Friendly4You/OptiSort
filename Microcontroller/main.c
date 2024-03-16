#include <xmc_common.h>
#include "XMC1100-LibHaas.h"
#include "HardwareController.h"

#define PRODUCTION_LINE_MAX_SPEED 15000

#define MAX_OBJECTS 100
int nextObjects[MAX_OBJECTS];
int numObjects = 0; // Tracks the number of objects in the array

// 1 if an object is currently being handled, 0 otherwise
int handleObjectMovingAway = 0;
int sortObjectMovingAway = 0;

void handleNewObject();
void sortObject();
void test(int onOrOff);

int main(void) {
	init();
	LEDStripes(OFF);
	productionLine(0, FORWARD);
	slider(0, SLIDERFORWARD);
	test(OFF);

	while (1U) {
		laser(ON);
		// New object handling
		if (lightBarrier2() == OFF && handleObjectMovingAway == 0) {
			handleNewObject();
		}

		// Object sorting if necessary
		if (lightBarrier1() == OFF && numObjects > 0
				&& sortObjectMovingAway == 0) {
			sortObject();
		}

		// Reset flags
		if (lightBarrier2() == ON) {
			handleObjectMovingAway = 0;
		}
		if (lightBarrier1() == ON) {
			sortObjectMovingAway = 0;
		}

		// Reset the slider if the button is pressed
		if (readButton1() == ON) {
			// slider to max backward position
			slider(50000, SLIDERBACKWARD);
			while (readInductivSensor1() == 0)
				;
			// slider stops
			slider(0, SLIDERFORWARD);
		}

		// reset system, if button 2 is pressed
		if (readButton2() == ON) {
			// wait till the button is released
			while (readButton2() == ON)
				;
			// reset the system
			NVIC_SystemReset();
		}

		productionLine(PRODUCTION_LINE_MAX_SPEED, FORWARD);
	}

	return 0; // Added return statement for formality, though it will never be reached in this loop
}

void handleNewObject() {
	handleObjectMovingAway = 1;

	productionLine(0, FORWARD);

	LEDStripes(ON);
	laser(OFF);

	pneumatics1(OUT);

	// Get classification
	uart_print("Good or bad?\n\r");

	// uart_wait_of_enter();
	while (uartEnterReceive == 0) {
		if (readButton2() == ON) {
			// wait till the button is released
			while (readButton2() == ON)
				;
			// reset the system
			NVIC_SystemReset();
		}
	}
	uartEnterReceive = 0;

	rs232_print(uartReceivedata);

	pneumatics1(IN);

	// Save if apple is good or bad in the nextObjects array (append to the end of the array)
	if (uartReceivedata[0] == 'n') {
		// do nothing
		rs232_print("Do nothing.");
	} else if (numObjects < MAX_OBJECTS) {
		nextObjects[numObjects++] = (
				uartReceivedata[0] == 'd' ? DONTSORTOUT : SORTOUT); // Fixed logic for good/bad apple
	}
	uart_clear_receivedata();

	laser(ON);
	LEDStripes(OFF);
	productionLine(PRODUCTION_LINE_MAX_SPEED, FORWARD);
}

void sortObject() {
	sortObjectMovingAway = 1;

	productionLine(0, FORWARD);
	if (nextObjects[0] == SORTOUT) {
		slider(50000, SLIDERFORWARD);
		while (readInductivSensor2() == 0)
			;
		slider(50000, SLIDERBACKWARD);
		while (readInductivSensor1() == 0)
			;
		// slider stops
		slider(0, SLIDERFORWARD);
	}
	// Shift the nextObjects array elements left, deleting the first element
	for (int i = 1; i < numObjects; i++) {
		nextObjects[i - 1] = nextObjects[i];
	}
	if (numObjects > 0)
		numObjects--; // Decrease the objects count after sorting one out
	productionLine(PRODUCTION_LINE_MAX_SPEED, FORWARD);
}

void test(int onOrOff) {
	if (!onOrOff) {
		// shut everything down
		laser(OFF);
		LEDStripes(OFF);
		redLED(OFF);
		greenLED(OFF);
		productionLine(0, FORWARD);
		return;
	}

	// light test: manual check
	laser(ON);
	LEDStripes(ON);
	redLED(ON);
	greenLED(ON);
	delay_ms(5000);
	LEDStripes(OFF);
	redLED(OFF);
	greenLED(OFF);
	delay_ms(2000);

	// check lightbarriers: automatic check
	if (lightBarrier1()) {
		greenLED(ON);
	} else {
		redLED(ON);
	}
	delay_ms(1000);
	redLED(OFF);
	greenLED(OFF);
	delay_ms(500);
	if (lightBarrier2()) {
		greenLED(ON);
	} else {
		redLED(ON);
	}
	delay_ms(1000);
	redLED(OFF);
	greenLED(OFF);
	delay_ms(500);
	if (lightBarrier3()) {
		greenLED(ON);
	} else {
		redLED(ON);
	}
	delay_ms(1000);
	redLED(OFF);
	greenLED(OFF);

	/*
	 // check productionLine
	 productionLine(PRODUCTION_LINE_MAX_SPEED, BACKWARD);
	 delay_ms(1000);
	 productionLine(PRODUCTION_LINE_MAX_SPEED, FORWARD);
	 delay_ms(1000);
	 productionLine(0, FORWARD);
	 */

	// check pneumatics1
	pneumatics1(OUT);
	delay_ms(1000);
	pneumatics1(IN);
	pneumatics2(OUT);
	delay_ms(1000);
	pneumatics2(IN);
}
