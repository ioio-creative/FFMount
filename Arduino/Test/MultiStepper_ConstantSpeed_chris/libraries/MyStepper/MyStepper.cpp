/*
  MyStepper.cpp - Library for encapsulating AccelStepper.
  Created by Christopher Wong, September 29, 2017.
  The MyStepper class depends heavily on the class AccelStepper.
  Released into the public domain.
*/

#include "MyStepper.h"


/* constructors */

MyStepper::MyStepper(uint8_t interface, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, bool enable){
    _accelStepper = AccelStepper(interface,  pin1,  pin2,  pin3,  pin4, enable);
}

MyStepper::MyStepper(uint8_t pin1, uint8_t pin2)
{
	_accelStepper = AccelStepper(AccelStepper::DRIVER, pin1, pin2);
}

/* end of constructors */


/* public methods */

bool MyStepper::myRun()
{
	bool runSpeedResult = false;

	// Use compareAbs to cater for negative distances
	if (!isCompleteTotDist())
	{
		runSpeedResult = _accelStepper.runSpeed();
	}
	
	long currTimeStampInMillis = millis();
	int timeStampDiffInMillis = currTimeStampInMillis - _lastTimeStampInMillis;
		
	if (timeStampDiffInMillis > _timeStepInMillis)
	{		
		if (!isCompleteHalfTotDist())
		{
			_accelStepper.setSpeed(speed() + _linearAccl * timeStampDiffInMillis * 0.001);
		}
		else
		{
			long anticipatedSpeed = speed() - _linearAccl * timeStampDiffInMillis * 0.001;
			// Use sgn(_linearAccl) to cater for negative distances, hence negative speeds
			if (sgn(_linearAccl) * anticipatedSpeed > 0)
			{
				_accelStepper.setSpeed(anticipatedSpeed);
			}
		}
		
		if (_isPrintTimeStepToSerial)
		{
			printStatusToSerial();
		}

		_lastTimeStampInMillis = currTimeStampInMillis;
	}
	
	return runSpeedResult;
}

float MyStepper::reset(long newPositionToGo, long totTime)
{
	long currPos = currentPosition();
	
	_totDist = newPositionToGo - currPos;
	_halfTotDist = _totDist / 2;
	_totTime = totTime;
	_linearAccl = computeLinearAccl(_totDist, totTime);
	_lastTimeStampInMillis = millis();
	_processStartTimeStampInMillis = _lastTimeStampInMillis;
	_processStartPosition = currPos;
		
	_accelStepper.setSpeed(0);
	_accelStepper.setMaxSpeed(computeMaxSpeedForLinearAccl(_totDist, totTime));	
	
	return _linearAccl;
}

void MyStepper::setTimeStepInMillis(int timeStep)
{
	_timeStepInMillis = timeStep;
}

void MyStepper::setIsPrintTimeStepToSerial(bool isPrintTimeStepToSerial)
{
	_isPrintTimeStepToSerial = isPrintTimeStepToSerial;
}

float MyStepper::speed()
{
	return _accelStepper.speed();
}

long MyStepper::currentPosition()
{
	return _accelStepper.currentPosition();
}

long MyStepper::getTotDist()
{
	return _totDist;
}

void MyStepper::printStatusToSerial()
{
	Serial.println(String(millis() - _processStartTimeStampInMillis, DEC) + ", " + String(currentPosition(), DEC) + ", " + String(speed(), DEC));
}

bool MyStepper::isCompleteTotDist()
{
	// Use compareAbs to cater for negative distances
	return compareAbs(currentPosition() - _processStartPosition, _totDist) != -1;
}

bool MyStepper::isCompleteHalfTotDist()
{
	// Use compareAbs to cater for negative distances
	return compareAbs(currentPosition() - _processStartPosition, _halfTotDist) != -1;
}




long MyStepper::distanceToGo()
{
    return _accelStepper.distanceToGo();
}

long MyStepper::targetPosition()
{
    return _accelStepper.targetPosition();
}

void MyStepper::setCurrentPosition(long position)
{
    _accelStepper.setCurrentPosition(position);
}

void MyStepper::stop()
{
    _accelStepper.stop();
}

void MyStepper::setSpeed(float speed)
{
    _accelStepper.setSpeed(speed);
}
/* end of public methods */


/* private methods */


void MyStepper::setMaxSpeed(float speed)
{
	_accelStepper.setMaxSpeed(speed);
}

float MyStepper::computeLinearAccl(long totDist, long totTime)
{
	return (4.0 * totDist) / (totTime * totTime);
}

float MyStepper::computeMaxSpeedForLinearAccl(long totDist, long totTime)
{
	return computeLinearAccl(totDist, totTime) * totTime / 2;
}

/* end of private methods */
