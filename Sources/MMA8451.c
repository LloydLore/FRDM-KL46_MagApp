/*
 * MMA8451.c
 *
 *  Created on: Sep 25, 2017
 *      Author: Michael.luo
 *      Note: I mainly copied the content of this file from Erich Styger. Special thanks to him. 
 *		I modified here to see if the files are synced. typed when in progress of 6th commit. 
 *		I modified here to see if the files are synced. typed when in progress of 7th commit. 
 *		During 7th commit, I succeed connecting the visual studio with CW eclipse through github. good thing. 
 *		I try push the 7th commit to the server. so I take the 8th commit. test again. again. again. 
 */

#include "MMA8451.h"
#include "I2C2.h"
#include "LEDR.h"
#include "LEDG.h"
#include "LEDG.h"

/* External 3-axis accelerometer control register addresses */
#define MMA8451_CTRL_REG_1	0x2AU
/* MMA8451 3-axis accelerometer control register bit masks */
#define MMA8451_ACTIVE_BIT_MASK	0x01U
#define MMA8451_F_READ_BIT_MASK	0x02U

/* External 3-axis accelerometer data register address */
#define MMA8451_OUT_X_MSB	0x01U
#define MMA8451_OUT_X_LSB	0x02U
#define MMA8451_OUT_Y_MSB	0x03U
#define MMA8451_OUT_Y_LSB	0x04U
#define MMA8451_OUT_Z_MSB	0x05U
#define MMA8451_OUT_Z_LSB	0x06U

static MMA8451_TDataState MMA8451_deviceData;

uint8_t MMA8451_ReadReg(uint8_t addr, uint8_t *data, short dataSize)	{
	uint8_t res;
	
	/* Send I2C address plus register address to the I2C bus *without* a stop condition */ 
	res = I2C2_MasterSendBlock(MMA8451_deviceData.handle, &addr, 1U, LDD_I2C_NO_SEND_STOP);
	if(res != ERR_OK){
		return ERR_FAILED;
	}
	while(!MMA8451_deviceData.dataTransmittedFlg)	{}	/* Wait until data is sent */
	MMA8451_deviceData.dataTransmittedFlg = FALSE;
	
	/* Receive InpData (1 byte) from the I2C bus and generates a stop condition to end transmission */
	res = I2C2_MasterReceiveBlock(MMA8451_deviceData.handle, data, dataSize, LDD_I2C_SEND_STOP);
	if(res != ERR_OK){
		return ERR_FAILED;
	}
	while(!MMA8451_deviceData.dataReceivedFlg)	{}		/* Wait until data is received */ 
	MMA8451_deviceData.dataReceivedFlg = FALSE;
	return ERR_OK;	
}

uint8_t MMA8451_WriteReg(uint8_t addr, uint8_t val)	{
	uint8_t buf[2], res;
	
	buf[0] = addr;
	buf[1] = val;
	
	res = I2C2_MasterSendBlock(MMA8451_deviceData.handle, &buf, 2U, LDD_I2C_SEND_STOP);	/* Send OutData (3 bytes with address) on the I2C bus and generates a stop condition to end transission */ 
	if(res != ERR_OK){
		return ERR_FAILED;
	}
	while(!MMA8451_deviceData.dataTransmittedFlg)	{}	/* Wait until data is send */
	MMA8451_deviceData.dataTransmittedFlg = FALSE;
	return ERR_OK;
}

static int8_t xyz[3];

void MMA8451_Run(void)	{
	uint8_t res;
	
	LEDR_On();
	LEDG_On();
	LEDB_On();
	MMA8451_deviceData.handle = I2C2_Init(&MMA8451_deviceData);
	
	/* F_READ: Fast read mode, data format limited to single byte (auto incremtnt counter will skip LSB)
	 * ACTIVE: Full scale selection
	 */
	res = MMA8451_WriteReg(MMA8451_CTRL_REG_1, MMA8451_F_READ_BIT_MASK|MMA8451_ACTIVE_BIT_MASK);
	if(res == ERR_OK)	{
		for(;;)	{
			res = MMA8451_ReadReg(MMA8451_OUT_X_MSB, (uint8_t*)&xyz, 3);
			LEDR_Put(xyz[0] > 50);
			LEDG_Put(xyz[1] > 50);
			LEDB_Put(xyz[2] > 50);
		}
	}
	LEDR_Off();
	LEDG_Off();
	LEDB_Off();
}
