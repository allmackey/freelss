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
	int sid = serialOpen ("/dev/ttyS0",UART_speed);
	float numStepsV = numSteps/853.333;
	char val[10];
	char cmd1[32];
	char cmd2[32];

        if (numStepsV>= 0.99) 
	{
		sprintf(val,"%d",(int) std::ceil(numStepsV));
		printf("%s %d\n","numStepsV:", (int) std::ceil(numStepsV));
	} else {
		sprintf(val,"%.2f",Round_off((double) numStepsV,2));
		printf("%s %.2f\n","numStepsV:", Round_off((double) numStepsV,2));
	}
        strcpy(cmd1, "G91 G1 S1 F1800 Y");
	serialPrintf(sid,"G91 G1 S1 F1800 Y");
	serialPrintf(sid,val);
	serialPrintf(sid,"\r\n");
	strcat(cmd1,val);
	strcat(cmd1,"\r\n");
	strcat(cmd1,"\0");
	sprintf(cmd2,"%s",cmd1);
        //serialPrintf(sid, cmd2);
        serialClose (sid);
        printf(cmd2);
	// Get the percent of a full revolution that theta is and convert that to number of steps
//	int numSteps = (theta / (2 * PI)) * m_stepsPerRevolution;
	printf("%s %f\n","theta:", theta);
	printf("%s %d\n","numSteps:", numSteps);
//	printf("%s %d\n","numStepsV:", numStepsV);
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


float Round_off(double N, double n) 
{ 
    int h; 
    double b, d, e, i, j, m, f; 
    b = N; 

  
    // Counting the no. of digits to the left of decimal point 
    // in the given no. 
    for (i = 0; b >= 1; ++i) 
        b = b / 10; 
  
    d = n - i; 
    b = N; 
    b = b * pow(10, d); 
    e = b + 0.5; 
    if ((float)e == (float)ceil(b)) { 
        f = (ceil(b)); 
        h = f - 2; 
        if (h % 2 != 0) { 
            e = e - 1; 
        } 
    } 
    j = floor(e); 
    m = pow(10, d); 
    j = j / m; 
    return j; 
} 

}
