#include <Wire.h>
#include <Servo.h>
#include "configure.h"
#include "ServoDecode.h"

long loopCount = 0;
Servo armServo;
Servo gripServo;
int courseTheta = 0;
int courseMagnitude = 0;

        int X  = 0;
	int Y  = 0;
	int ROT = 0;
	int AUX = 0;
	bool L_UP_pushed = false;
	bool L_DWN_pushed = false;
	bool R_UP_pushed = false;
	bool R_DWN_pushed = false;
	int servoTmp = 90;

void setup()
{
	Wire.begin();  
	Serial.begin(9600);
        ServoDecode.begin();
        ServoDecode.setFailsafe(1,1500); // set channel 1 failsafe pulsewidth. right horizontal stick
        ServoDecode.setFailsafe(2,1500); // set channel 2 failsafe pulsewidth. right vertical stick
        ServoDecode.setFailsafe(3,1500); // set channel 3 failsafe pulsewidth. left vertical stick
        ServoDecode.setFailsafe(4,1500); // set channel 4 failsafe pulsewidth. left horizontal stick
        ServoDecode.setFailsafe(5,1500); // set channel 5 failsafe pulsewidth. left buttons
        ServoDecode.setFailsafe(6,1500); // set channel 6 failsafe pulsewidth. right buttons
	pinMode(6, OUTPUT);
	digitalWrite(6, HIGH);
	Serial.println("setup() entered.");
  
	setAutoSpeedRegulationOn(PRIMARY_MD25_ADDR);
	setAutoSpeedRegulationOn(SECONDARY_MD25_ADDR);
	setTimeoutOff(PRIMARY_MD25_ADDR);
	setTimeoutOff(SECONDARY_MD25_ADDR);
	setMD25SpeedByteFormat(PRIMARY_MD25_ADDR, 1);
	setMD25SpeedByteFormat(SECONDARY_MD25_ADDR, 1);

	armServo.attach(ARM_SERVO_PIN);
	gripServo.attach(GRIPPER_SERVO_PIN);
	armServo.write(90); // center servos
	gripServo.write(90);
	
	Serial.println("End of Setup()");
	Serial.print("Loop:");
	Serial.println(loopCount);
	digitalWrite(6, LOW);
}

void loop()
{  
	loopCount++;  
	Serial.print("Loop:");
	Serial.println(loopCount);
	// speed  -128 (Full Left)   0 (Stop)   127 (Full Right)
	// speed  -128 (Full Reverse)   0 (Stop)   127 (Full Forward)
	// speed  -128 (Full ClockWise)   0 (Stop)   127 (Full CounterClockwise)

	

	X  = ServoDecode.GetChannelPulseWidth(4); // left horizontal stick
	Y  = ServoDecode.GetChannelPulseWidth(3); // left vertical stick
	ROT = ServoDecode.GetChannelPulseWidth(1); // right horizontal stick  
	moveJoystick(750, 2250, X, 750, 2250, Y, 750, 2250, ROT);

	AUX = map(ServoDecode.GetChannelPulseWidth(2), 750, 2250, -127, 127);
	// do something here with the extra AUX analog channel, right verticle stick.

	if(L_UP_pushed = (ServoDecode.GetChannelPulseWidth(5) < 1500-200) &&
	 (servoTmp = gripServo.read()) < 180)// open grip commanded and grip not already fully open
	{gripServo.write(servoTmp+1);}
	if(L_DWN_pushed = (ServoDecode.GetChannelPulseWidth(5) > 1500+200) &&
	 (servoTmp = gripServo.read()) > 0)// close grip commanded and grip not already fully closed
	{gripServo.write(servoTmp-1);}
	if(R_UP_pushed = (ServoDecode.GetChannelPulseWidth(6) < 1500-200) &&
	 (servoTmp = armServo.read()) < 180)// raise arm commanded and arm not already fully raised
	{armServo.write(servoTmp+1);}
	if(R_DWN_pushed = (ServoDecode.GetChannelPulseWidth(6) > 1500+200) &&
	 (servoTmp = armServo.read()) > 0)// lower arm commanded and arm not already fully lowered
	{armServo.write(servoTmp-1);}  
	
	//illuminate LED on any button push
	if(L_UP_pushed || L_DWN_pushed || R_UP_pushed || R_DWN_pushed)
	{digitalWrite(6, HIGH);}
	else
	{digitalWrite(6, LOW);} 

    for (int i = 0; i <= 6 ; i++)
    {
        Serial.print("RC channel ");
        Serial.print(i);
        Serial.print(": ");
	Serial.println(ServoDecode.GetChannelPulseWidth(i));
    }
}

