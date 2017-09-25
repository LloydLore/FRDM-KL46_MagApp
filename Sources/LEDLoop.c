/*
 * LEDLoop.c
 *
 *  Created on: Sep 25, 2017
 *      Author: Michael.luo
 */

#include "LEDLoop.h"

void LED_Flash_Loop(void)	{
	WAIT1_Waitms(100);
	LEDR_Neg();
	WAIT1_Waitms(200);
	LEDB_Neg();
	WAIT1_Waitms(400);
	LEDG_Neg();
}

