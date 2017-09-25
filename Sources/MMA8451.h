/*
 * MMA8451.h
 *
 *  Created on: Sep 25, 2017
 *      Author: Michael.luo
 *      Note: I copied this file mainly from Erich Styger. Special thanks to him. 
 */

#ifndef MMA8451_H_
#define MMA8451_H_

#include "PE_Types.h"
#include "PE_LDD.h"

typedef struct	{
	volatile bool dataReceivedFlg;			/* Set to TRUE by the interrupt if we have received data */
	volatile bool dataTransmittedFlg;		/* Set to TRUE by the interrupt if we have sent data */
	LDD_TDeviceData *handle;				/* pointer to the device handle */
} MMA8451_TDataState;

void MMA8451_Run(void);

#endif /* MMA8451_H_ */
