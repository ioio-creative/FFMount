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
    
    typedef enum
    {
        FUNCTION  = 0, ///< Use the functional interface, implementing your own driver functions (internal use only)
        DRIVER    = 1, ///< Stepper Driver, 2 driver pins required
        FULL2WIRE = 2, ///< 2 wire stepper, 2 motor pins required
        FULL3WIRE = 3, ///< 3 wire stepper, such as HDD spindle, 3 motor pins required
        FULL4WIRE = 4, ///< 4 wire full stepper, 4 motor pins required
        HALF3WIRE = 6, ///< 3 wire half stepper, such as HDD spindle, 3 motor pins required
        HALF4WIRE = 8  ///< 4 wire half stepper, 4 motor pins required
    } MotorInterfaceType;
    
    MyStepper(uint8_t interface = MyStepper::FULL4WIRE, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, bool enable = true);
    
    MyStepper(uint8_t pin1, uint8_t pin2);	
	
	bool myRun();
	
	float reset(long newPositionToGo, float totTime);	
	
	void setTimeStepInMillis(int timeStep);
	
	void setIsPrintTimeStepToSerial(bool isPrintTimeStep);	
	
	float speed();
	
	float maxSpeed();
	
	long currentPosition();
	
	long getTotDist();
	
	void printStatusToSerial();
	
	bool isCompleteTotDist();
	
	bool isCompleteHalfTotDist();
	
	void setMinReturnSpeed(float speed);
	
	long getTimeSinceLastResetInMillis();
    
    //From AcceleStepper
    long distanceToGo();
    void setCurrentPosition(long position);
    long targetPosition();
    void stop();
    void setSpeed(float speed);
	
  private:
    AccelStepper _accelStepper;
	long _totDist;
	long _halfTotDist;
	float _totTime;
	float _halfTotTimeInMillis;
	float _linearAccl;
	int _signOfLinearAccl;
	int _timeStepInMillis;
	long _lastTimeStampInMillis;
	bool _isPrintTimeStepToSerial;
	long _processStartTimeStampInMillis;
	long _processStartPosition;
	float _minReturnSpeed;
	float _theoreticalMaxSpeed;
	float _lastSpeed;
	long _nextUpdateTimeStepInMillis;

	void setMaxSpeed(float speed);

	float computeLinearAccl(long totDist, float totTime);
	float computeMaxSpeedForLinearAccl(long totDist, float totTime);
	float computeSpeedByTimeSinceLastReset();
	float computeSpeedByTimeSinceLastReset(long currTimeStampInMillis);
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
