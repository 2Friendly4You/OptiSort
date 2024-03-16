#ifndef Hardware_Controller_h_
#define Hardware_Controller_h_

#define LED_TOLERANCE_MILLIS 5

#define redLEDpin 2
#define redLEDport 1

#define greenLEDpin 3
#define greenLEDport 1

#define LEDstripespin 4
#define LEDstripesport 0

#define lightbarrier1pin 3
#define lightbarrier1port 2

#define lightbarrier2pin 2
#define lightbarrier2port 2

#define lightbarrier3pin 1
#define lightbarrier3port 2

#define laserpin 0
#define laserport 0

#define pneumatics1pin 1
#define pneumatics1port 0

#define pneumatics2pin 2
#define pneumatics2port 0

#define button1pin 5
#define button1port 1

#define button2pin 4
#define button2port 1

#define dir1pin 7
#define dir1port 0

#define pwm1pin 6
#define pwm1port 0

#define inductivSensor1Pin 10
#define inductivSensor1Port 0

#define inductivSensor2Pin 11
#define inductivSensor2Port 0

#define productionLineDirectionPin 9
#define productionLineDirectionPort 0

#define ON 1
#define OFF 0
#define FORWARD 0
#define BACKWARD 1
#define OUT 1
#define IN 0
#define SLIDERBACKWARD 1
#define SLIDERFORWARD 0
#define SORTOUT 1
#define DONTSORTOUT 0

extern void init(void);

extern void laser(int onOrOff);
extern void LEDStripes(int onOrOff);
extern void redLED(int onOrOff);
extern void greenLED(int onOrOff);
extern void productionLine(int velocity, int direction);
extern int lightBarrier1(void);
extern int lightBarrier2(void);
extern int lightBarrier3(void);
extern void pneumatics1(int outOrIn);
extern void pneumatics2(int outOrIn);
extern int readButton1(void);
extern int readButton2(void);
extern int readInductivSensor1(void);
extern int readInductivSensor2(void);
extern void slider(int velocity, int direction);

#endif
