#include "XMC1100-LibHaas.h"
#include "HardwareController.h"

void init() {
	uart_init();
	uart_interrupt_enable();
	uart_select_channel(UARTCHANNEL_0);

	rs232_init();
	rs232_interrupt_enable();

	bit_init(redLEDport, redLEDpin, OUTP);
	bit_init(greenLEDport, greenLEDpin, OUTP);
	bit_init(LEDstripesport, LEDstripespin, OUTP);
	bit_init(laserport, laserpin, OUTP);
	bit_init(lightbarrier1port, lightbarrier1pin, INP);
	bit_init(lightbarrier2port, lightbarrier2pin, INP);
	bit_init(lightbarrier3port, lightbarrier3pin, INP);
	bit_init(pneumatics1port, pneumatics1pin, OUTP);
	bit_init(pneumatics2port, pneumatics2pin, OUTP);
	bit_init_mode(button1port, button1pin, INP, PULLDOWN);
	bit_init_mode(button2port, button2pin, INP, PULLDOWN);
	bit_init(inductivSensor1Port, inductivSensor1Pin, INP);
	bit_init(inductivSensor2Port, inductivSensor2Pin, INP);
	bit_init(dir1port, dir1pin, OUTP);
	bit_init(productionLineDirectionPort, productionLineDirectionPin, OUTP);

	pwm1_init();
	pwm1_start();
	pwm3_init();
	pwm3_start();

	productionLine(0, FORWARD);
	slider(0, SLIDERFORWARD);

	bit_write(P0, laserport, OFF);
	bit_write(LEDstripesport, LEDstripespin, OFF);
}

void laser(int onOrOff) {
	onOrOff ?
			bit_write(laserport, laserpin, ON) :
			bit_write(laserport, laserpin, OFF);
}

void LEDStripes(int onOrOff) {
	onOrOff ?
			bit_write(LEDstripesport, LEDstripespin, OFF) :
			bit_write(LEDstripesport, LEDstripespin, ON);
}

void redLED(int onOrOff) {
	onOrOff ?
			bit_write(redLEDport, redLEDpin, OFF) :
			bit_write(redLEDport, redLEDpin, ON);
}

void greenLED(int onOrOff) {
	onOrOff ?
			bit_write(greenLEDport, greenLEDpin, OFF) :
			bit_write(greenLEDport, greenLEDpin, ON);
}

/*
void productionLine(int onOrOff, int direction) {
	onOrOff ?
			bit_write(relay1port, relay1pin, OFF) :
			bit_write(relay1port, relay1pin, ON);
	direction ?
			bit_write(relay2port, relay2pin, ON) :
			bit_write(relay2port, relay2pin, OFF);
}
*/

void productionLine(int velocity, int direction) {
	direction ?
			bit_write(productionLineDirectionPort, productionLineDirectionPin, BACKWARD) :
			bit_write(dir1port, dir1pin, FORWARD);
	// inverse velocity by 50000
	pwm3_duty_cycle(50000 - velocity);
}


int lightBarrier1(void) {
	int firstActivation = bit_read(lightbarrier1port, lightbarrier1pin);
	delay_ms(LED_TOLERANCE_MILLIS);
	int secondActivation = bit_read(lightbarrier1port, lightbarrier1pin);

	// implements tolerance

	while (firstActivation != secondActivation) {
		firstActivation = secondActivation;
		delay_ms(LED_TOLERANCE_MILLIS);
		secondActivation = bit_read(lightbarrier1port, lightbarrier1pin);
	}

	if (firstActivation == 0) {
		return ON;
	} else {
		return OFF;
	}
}

int lightBarrier2(void) {
	int firstActivation = bit_read(lightbarrier2port, lightbarrier2pin);
	delay_ms(LED_TOLERANCE_MILLIS);
	int secondActivation = bit_read(lightbarrier2port, lightbarrier2pin);

	// implements tolerance

	while (firstActivation != secondActivation) {
		firstActivation = secondActivation;
		delay_ms(LED_TOLERANCE_MILLIS);
		secondActivation = bit_read(lightbarrier2port, lightbarrier2pin);
	}

	if (firstActivation == 0) {
		return ON;
	} else {
		return OFF;
	}
}

int lightBarrier3(void) {
	int firstActivation = bit_read(lightbarrier3port, lightbarrier3pin);
	delay_ms(LED_TOLERANCE_MILLIS);
	int secondActivation = bit_read(lightbarrier3port, lightbarrier3pin);

	// implements tolerance

	while (firstActivation != secondActivation) {
		firstActivation = secondActivation;
		delay_ms(LED_TOLERANCE_MILLIS);
		secondActivation = bit_read(lightbarrier3port, lightbarrier3pin);
	}

	if (firstActivation == 0) {
		return ON;
	} else {
		return OFF;
	}
}

void pneumatics1(int outOrIn) {
	outOrIn ?
			bit_write(pneumatics1port, pneumatics1pin, OUT) :
			bit_write(pneumatics1port, pneumatics1pin, IN);
}

void pneumatics2(int outOrIn) {
	outOrIn ?
			bit_write(pneumatics2port, pneumatics2pin, OUT) :
			bit_write(pneumatics2port, pneumatics2pin, IN);
}

int readButton1(void) {
	return !bit_read(button1port, button1pin);
}

int readButton2(void) {
	return !bit_read(button2port, button2pin);
}

int readInductivSensor1(void) {
	return bit_read(inductivSensor1Port, inductivSensor1Pin);
}

int readInductivSensor2(void) {
	return bit_read(inductivSensor2Port, inductivSensor2Pin);
}

void slider(int velocity, int direction) {
	direction ?
			bit_write(dir1port, dir1pin, SLIDERBACKWARD) :
			bit_write(dir1port, dir1pin, SLIDERFORWARD);
	// inverse velocity by 50000
	pwm1_duty_cycle(50000 - velocity);
}
