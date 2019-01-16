/*
 ****************************************************************************
 *  Copyright (c) 2014 Uriah Liggett <freelaserscanner@gmail.com>           *
 *	This file is part of FreeLSS.                                           *
 *                                                                          *
 *  FreeLSS is free software: you can redistribute it and/or modify         *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, either version 3 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  FreeLSS is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with FreeLSS.  If not, see <http://www.gnu.org/licenses/>.       *
 ****************************************************************************
*/

#include "Main.h"
#include "A4988TurnTable.h"
#include "Thread.h"
#include "Setup.h"
#include "PresetManager.h"
#include "wiringSerial.h"

namespace freelss
{

A4988TurnTable::A4988TurnTable()
{
	Setup * setup = Setup::get();
	m_responseDelay = setup->motorResponseDelay;
	m_stepDelay = setup->motorStepDelay;
	m_enablePin = setup->motorEnablePin;
	m_stepPin = setup->motorStepPin;
	m_directionPin = setup->motorDirPin;
	m_stepsPerRevolution = setup->stepsPerRevolution;
	m_stabilityDelay = PresetManager::get()->getActivePreset().stabilityDelay;
}

A4988TurnTable::~A4988TurnTable()
{
	// Disable the stepper
	digitalWrite(m_enablePin, HIGH);
	Thread::usleep(m_responseDelay);
}

void A4988TurnTable::initialize()
{
	Setup * setup = Setup::get();
	int responseDelay = setup->motorResponseDelay;
	int enablePin = setup->motorEnablePin;
	int stepPin = setup->motorStepPin;
	int directionPin = setup->motorDirPin;

	// Disable the stepper
	pinMode(enablePin, OUTPUT);
	digitalWrite (enablePin, HIGH);

	// Put us in a known state
	pinMode(stepPin, OUTPUT);
	digitalWrite (stepPin, LOW);

	pinMode(directionPin, OUTPUT);
	digitalWrite (directionPin, LOW);

	Thread::usleep(responseDelay);

	//int sid = serialOpen ("/dev/ttyS0", 9600);
	char cmd1[100];
        strcpy(cmd1, "Start UART\r\n");
        //serialPrintf(sid, cmd1);
        //serialClose (sid);
	printf(cmd1);
}

void A4988TurnTable::step()
{
        //int sid = serialOpen ("/dev/ttyS0", 9600);
        //char cmd1[100];
        //strcpy(cmd1, "G1 S1 Y.1\r\n");
        //serialPrintf(sid, cmd1);
        //serialClose (sid);
        //printf(cmd1);

	digitalWrite(m_stepPin, LOW);
	Thread::usleep(m_responseDelay);

	digitalWrite(m_stepPin, HIGH);
	Thread::usleep(m_responseDelay);

	// Wait the step delay (this is how speed is controlled)
	Thread::usleep(m_stepDelay);
}

int A4988TurnTable::rotate(real theta)
{
	int numSteps = (theta / (2 * PI)) * m_stepsPerRevolution;
	int sid = serialOpen ("/dev/ttyS0",115200 );
	float numStepsVc = numSteps/853.333;
	char val[10];
	int numStepsV = numSteps/853.333;
        if (numStepsVc >= 1.0) 
	{	
		sprintf(val,"%d",ceil(numStepsVc));
	} else {
		sprintf(val,"%f",numStepsVc);
	}
	char cmd1[100];
        strcpy(cmd1, "G91 G1 S1 F1800 Y");
	strcat(cmd1,val);
	strcat(cmd1,"\r\n");
        //serialPrintf(sid, cmd1);
        serialClose (sid);
        printf(cmd1);
	// Get the percent of a full revolution that theta is and convert that to number of steps
//	int numSteps = (theta / (2 * PI)) * m_stepsPerRevolution;
	printf("%s %f\n","theta:", theta);
	printf("%s %d\n","numSteps:", numSteps);
	printf("%s %d\n","numStepsV:", numStepsV);
	printf("%s %d\n","Steps Per Rev:", m_stepsPerRevolution);
// Step the required number of steps
//	for (int i = 0; i < numSteps; i++)
//	{
//		step();
//	}

	// Sleep the stability delay amount
	Thread::usleep(m_stabilityDelay);

	return numSteps;
}

void A4988TurnTable::setMotorEnabled(bool enabled)
{
	int value = enabled ? LOW : HIGH;

	digitalWrite (m_enablePin, value);
	Thread::usleep(m_responseDelay);
}

}
