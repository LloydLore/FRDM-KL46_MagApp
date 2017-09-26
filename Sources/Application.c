/*
 * Application.c
 *
 *  Created on: Sep 18, 2017
 *      Author: Michael.luo
 */

#include "Application.h"
#include "RxBuf.h"
#include "AS1.h"
#include "LEDLoop.h"
/* header files used below are used to merge APP_Run and MMA8451_Run() together. */
#include "MMA8451.h"			
#include "I2C2.h"

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

uint8_t MMA8451_ReadReg(uint8_t addr, uint8_t *data, short dataSize) {
	uint8_t res;

	/* Send I2C address plus register address to the I2C bus *without* a stop condition */
	res = I2C2_MasterSendBlock(MMA8451_deviceData.handle, &addr, 1U, LDD_I2C_NO_SEND_STOP);
	if (res != ERR_OK) {
		return ERR_FAILED;
	}
	while (!MMA8451_deviceData.dataTransmittedFlg) {}	/* Wait until data is sent */
	MMA8451_deviceData.dataTransmittedFlg = FALSE;

	/* Receive InpData (1 byte) from the I2C bus and generates a stop condition to end transmission */
	res = I2C2_MasterReceiveBlock(MMA8451_deviceData.handle, data, dataSize, LDD_I2C_SEND_STOP);
	if (res != ERR_OK) {
		return ERR_FAILED;
	}
	while (!MMA8451_deviceData.dataReceivedFlg) {}		/* Wait until data is received */
	MMA8451_deviceData.dataReceivedFlg = FALSE;
	return ERR_OK;
}

uint8_t MMA8451_WriteReg(uint8_t addr, uint8_t val) {
	uint8_t buf[2], res;

	buf[0] = addr;
	buf[1] = val;

	res = I2C2_MasterSendBlock(MMA8451_deviceData.handle, &buf, 2U, LDD_I2C_SEND_STOP);	/* Send OutData (3 bytes with address) on the I2C bus and generates a stop condition to end transission */
	if (res != ERR_OK) {
		return ERR_FAILED;
	}
	while (!MMA8451_deviceData.dataTransmittedFlg) {}	/* Wait until data is send */
	MMA8451_deviceData.dataTransmittedFlg = FALSE;
	return ERR_OK;
}

static int8_t xyz[3];
 
static UART_Desc deviceData;
 
static void SendChar(unsigned char ch, UART_Desc *desc) {
		  desc->isSent = FALSE;  /* this will be set to 1 once the block has been sent */
  while(AS1_SendBlock(desc->handle, (LDD_TData*)&ch, 1)!=ERR_OK) {} /* Send char */
  while(!desc->isSent) {} /* wait until we get the green flag from the TX interrupt */
}
 
static void SendString(const unsigned char *str,  UART_Desc *desc) {
  while(*str!='\0') {
	SendChar(*str++, desc);
  }
}
 
static void Init(void) {

	/* initialize I2c interface. */
	MMA8451_deviceData.handle = I2C2_Init(&MMA8451_deviceData);
  /* initialize struct fields */
  deviceData.handle = AS1_Init(&deviceData);
  deviceData.isSent = FALSE;
  deviceData.rxChar = '\0';
  deviceData.rxPutFct = RxBuf_Put;
  /* set up to receive RX into input buffer */
  RxBuf_Init(); /* initialize RX buffer */
  /* Set up ReceiveBlock() with a single byte buffer. We will be called in OnBlockReceived() event. */
  while(AS1_ReceiveBlock(deviceData.handle, (LDD_TData *)&deviceData.rxChar, sizeof(deviceData.rxChar))!=ERR_OK) {} /* initial kick off for receiving data */
}
 
void APP_Run(void) {
	uint8_t res;
	LEDR_On();
	LEDG_On();
	LEDB_On();
	
  Init();
  SendString((unsigned char*)"Hello World\r\n", &deviceData);
  res = MMA8451_WriteReg(MMA8451_CTRL_REG_1, MMA8451_F_READ_BIT_MASK | MMA8451_ACTIVE_BIT_MASK);
  if (res == ERR_OK) {
	  for (;;) {
		  if (RxBuf_NofElements() != 0) {
			  SendString((unsigned char*)"echo: ", &deviceData);
			  while (RxBuf_NofElements() != 0) {
				  unsigned char ch;

				  (void)RxBuf_Get(&ch);
				  SendChar(ch, &deviceData);
			  }
			  SendString((unsigned char*)"\r\n", &deviceData);
		  }
		  //res = MMA8451_WriteReg(MMA8451_CTRL_REG_1, MMA8451_F_READ_BIT_MASK | MMA8451_ACTIVE_BIT_MASK);

		  //LED_Flash_Loop();
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
