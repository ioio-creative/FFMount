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
	
	/* long currTimeStampInMillis = millis();
	int timeStampDiffInMillis = currTimeStampInMillis - _lastTimeStampInMillis;
		
	if (timeStampDiffInMillis > _timeStepInMillis)
	{			
		_lastSpeed = speed();  // needed in computeSpeedByTimeSinceLastReset()

		if (!isCompleteHalfTotDist())
		{			
			_accelStepper.setSpeed(_lastSpeed + _linearAccl * timeStampDiffInMillis * 0.001);
		}
		else
		{
			float anticipatedSpeed = _lastSpeed - _linearAccl * timeStampDiffInMillis * 0.001;			
			if (_signOfLinearAccl * anticipatedSpeed > _minReturnSpeed)  // Use _signOfLinearAccl to cater for negative distances, hence negative speeds
			{
				_accelStepper.setSpeed(anticipatedSpeed);
			}
		}		

		if (_isPrintTimeStepToSerial)
		{
			printStatusToSerial();
		}
		
		_lastTimeStampInMillis = currTimeStampInMillis;
	} */
	
	long currTimeStampInMillis = millis();
	if (currTimeStampInMillis - _processStartTimeStampInMillis > _nextUpdateTimeStepInMillis)
	{		
		_lastSpeed = speed();  // needed in computeSpeedByTimeSinceLastReset()
		_accelStepper.setSpeed(computeSpeedByTimeSinceLastReset(currTimeStampInMillis));
		_nextUpdateTimeStepInMillis += _timeStepInMillis;
		
		if (_isPrintTimeStepToSerial)
		{
			printStatusToSerial();
		}
	}
	
	return runSpeedResult;
}

float MyStepper::reset(long newPositionToGo, float totTime)
{
	long currPos = currentPosition();
	
	_totDist = newPositionToGo - currPos;
	_halfTotDist = _totDist / 2;
	_totTime = totTime;
	_halfTotTimeInMillis = totTime * 0.5 * 1000;
	_linearAccl = computeLinearAccl(_totDist, totTime);
	_signOfLinearAccl = sgn(_linearAccl);
	_lastTimeStampInMillis = millis();
	_processStartTimeStampInMillis = _lastTimeStampInMillis;
	_processStartPosition = currPos;
	_nextUpdateTimeStepInMillis = _timeStepInMillis;
		
	_accelStepper.setSpeed(0);
	_lastSpeed = 0;
	_theoreticalMaxSpeed = computeMaxSpeedForLinearAccl(_totDist, totTime);
	_accelStepper.setMaxSpeed(_theoreticalMaxSpeed);
	
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

float MyStepper::maxSpeed()
{
	return _accelStepper.maxSpeed();
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

void MyStepper::setMinReturnSpeed(float speed)
{
	_minReturnSpeed = speed;
}

long MyStepper::getTimeSinceLastResetInMillis()
{
	return millis() - _processStartTimeStampInMillis;
}


/* From AccelStepper */

long MyStepper::distanceToGo()
{
    //return _accelStepper.distanceToGo();
	return _totDist - (currentPosition() - _processStartPosition);
}

long MyStepper::targetPosition()
{
    //return _accelStepper.targetPosition();
	return _processStartPosition + _totDist;
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

/* end of From AccelStepper */

/* end of public methods */


/* private methods */


void MyStepper::setMaxSpeed(float speed)
{
	_accelStepper.setMaxSpeed(speed);
}

float MyStepper::computeLinearAccl(long totDist, float totTime)
{
	return (4.0 * totDist) / (totTime * totTime);
}

float MyStepper::computeMaxSpeedForLinearAccl(long totDist, float totTime)
{
	return computeLinearAccl(totDist, totTime) * totTime * 0.5;
}

float MyStepper::computeSpeedByTimeSinceLastReset()
{
	return computeSpeedByTimeSinceLastReset(millis());
}

float MyStepper::computeSpeedByTimeSinceLastReset(long currTimeStampInMillis)
{	
	long timeInMillisSinceLastReset = currTimeStampInMillis - _processStartTimeStampInMillis;
	if (timeInMillisSinceLastReset < _halfTotTimeInMillis)  // accelerate phase
	{
		return _linearAccl * timeInMillisSinceLastReset * 0.001;
	}
	else  // decelerate phase
	{
		float anticipatedSpeed = _theoreticalMaxSpeed - (timeInMillisSinceLastReset - _halfTotTimeInMillis) * _linearAccl * 0.001;
		
		// Use _signOfLinearAccl to cater for negative distances, hence negative speeds
		if (_signOfLinearAccl * anticipatedSpeed > _minReturnSpeed)
		{
			return anticipatedSpeed;
		}
		else
		{
			if (abs(_lastSpeed) < abs(_minReturnSpeed))
			{
				return _signOfLinearAccl * _minReturnSpeed;
			}
			else
			{
				return _lastSpeed;
			}
		}
	}	
}

/* end of private methods */
