/*
  MyStepper.h - Library for encapsulating AccelStepper.
  Created by Christopher Wong, September 29, 2017.
  The MyStepper class depends heavily on the class AccelStepper.
  Released into the public domain.
*/
#ifndef MyStepper_h
#define MyStepper_h

#include "Arduino.h"
#include <AccelStepper.h>

class MyStepper
{
  public:
    MyStepper(uint8_t pin1, uint8_t pin2);	
	
	bool myRun();
	
	float reset(long newPositionToGo, long totTime);	
	
	void setTimeStepInMillis(int timeStep);
	
	void setIsPrintTimeStepToSerial(bool isPrintTimeStep);	
	
	float speed();
	
	void maxSpeed();
	
	long currentPosition();
	
	long getTotDist();
	
	void printStatusToSerial();
	
	bool isCompleteTotDist();
	
	bool isCompleteHalfTotDist();
	
  private:
    AccelStepper _accelStepper;
	long _totDist;
	long _halfTotDist;
	long _totTime;
	long _halfTotTime;
	long _linearAccl;
	int _timeStepInMillis;
	long _lastTimeStampInMillis;
	bool _isPrintTimeStepToSerial;
	long _processStartTimeStampInMillis;
	long _processStartPosition;
	
	void setSpeed(float speed);
	void setMaxSpeed(float speed);
	void setCurrentPosition(long position);	
	
	float computeLinearAccl(long totDist, long totTime);
	float computeMaxSpeedForLinearAccl(long totDist, long totTime);	
};

// http://forum.arduino.cc/index.php?topic=37804.0
static inline int8_t sgn(long val)
{
	if (val < 0)
	{
		return -1;
	}
	else if (val == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static inline int8_t compareAbs(long val1, long val2)
{
	if (abs(val1) < abs(val2))
	{
		return -1;
	}
	else if (val1 == val2)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

#endif