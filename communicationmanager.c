/*
 * communicationmanager.c
 *
 *  Created on: Sep 13, 2019
 *      Author: Alex Morin
 */

#include "communicationmanager.h"

uint8_t key2[16] = { (uint8_t)0x2b, (uint8_t)0x7e, (uint8_t)0x15, (uint8_t)0x16, (uint8_t)0x28, (uint8_t)0xae, (uint8_t)0xd2, (uint8_t)0xa6, (uint8_t)0xab, (uint8_t)0xf7, (uint8_t)0x15, (uint8_t)0x88, (uint8_t)0x09, (uint8_t)0xcf, (uint8_t)0x4f, (uint8_t)0x3c };

#define FSM_STATE_HEADER  0x00
#define FSM_STATE_PID     0x01
#define FSM_STATE_CMD     0x02
#define FSM_STATE_PLL     0x03
#define FSM_STATE_PL      0x04
#define FSM_STATE_CRC     0x05

 //Local variables
oCommunicationManager_t oCommunicationManager;

/**
  * @brief  Calculate a CRC16 on a buffer
  * @param  pu8Data: Pointers to our data for CRC calculation
  * @param  u16Length: Length of buffer to calculate our CRC16 on
  * @retval the calculated CRC16
  */
uint16_t CommunicationManagerCRC16(uint8_t* pu8Data, uint16_t u16Length) {
	uint8_t i;
	uint16_t wCrc = 0xffff;
	while (u16Length--)
	{
		wCrc ^= *(unsigned char*)pu8Data++ << 8;
		for (i = 0; i < 8; i++)
			wCrc = wCrc & 0x8000 ? (wCrc << 1) ^ 0x1021 : wCrc << 1;
	}
	return wCrc & 0xffff;
}


/**
  * @brief  Format and send return message containing a payload
  * @param  u8CMD: The cmd code value
  * @param  pu8Payload: Pointer to a uint8_t buffer containing data to send
  * @param  u16PayloadLength: uint16_t for the buffer length
  */
void CommunicationManagerSendPacket(uint8_t u8CMD, uint8_t* pu8Payload, uint16_t u16PayloadLength) {
	uint8_t u8Reply[0xFF];
	uint16_t u16CalculatedCRC16;

	if (pu8Payload == NULL && u16PayloadLength != 0) {
		return;
	}

	u8Reply[0] = START;						//START
	u8Reply[1] = oCommunicationManager.u8PID;	//PID
	u8Reply[2] = u8CMD;						//CMD
	u8Reply[3] = u16PayloadLength;			//PLL
	if (u16PayloadLength != 0) {
		memcpy(&u8Reply[4], pu8Payload, u16PayloadLength);	//PL
	}
	u16CalculatedCRC16 = CommunicationManagerCRC16(u8Reply, 4 + u16PayloadLength);
	u8Reply[4 + u16PayloadLength] = (uint8_t)(u16CalculatedCRC16 >> 8);		//CRC part 1
	u8Reply[5 + u16PayloadLength] = (uint8_t)u16CalculatedCRC16;			//CRC part 2
	oCommunicationManager.u8PID++;

	// TODO: Encrypt and send data
	printf("     Sending (%02d): ", 6 + u16PayloadLength);

	unsigned char i;
	for (i = 0; i < 6 + u16PayloadLength; ++i)
		printf("0x%.2X ", u8Reply[i]);
	printf("\n");
}


uint8_t CommunicationManagerProcessChar(uint8_t u8Data, uint8_t* pu8Data, uint16_t* pu16Length) {
	uint16_t u16Index;
	uint16_t u16CircularBufferIndex;
	uint16_t u16RegularIndex;
	uint16_t u16CalculatedCRC16;

	if (pu8Data == NULL || pu16Length == NULL) {
		return 0;
	}

	switch (oCommunicationManager.u8State) {
	case FSM_STATE_HEADER:
		if (u8Data == START) {
			oCommunicationManager.u8State = FSM_STATE_PID;
			oCommunicationManager.u8CounterCRC = 2;
			oCommunicationManager.u8PayloadLength = 0;
			oCommunicationManager.u16Counter = 0;
			oCommunicationManager.u8LastDataReceived[oCommunicationManager.u16Counter] = u8Data;
			oCommunicationManager.u16Counter++;
		}
		break;
	case FSM_STATE_PID:
			oCommunicationManager.u8State = FSM_STATE_CMD;
			oCommunicationManager.u8LastDataReceived[oCommunicationManager.u16Counter] = u8Data;
			oCommunicationManager.u16Counter++;
		break;
	case FSM_STATE_CMD:
			oCommunicationManager.u8State = FSM_STATE_PLL;
			oCommunicationManager.u8LastDataReceived[oCommunicationManager.u16Counter] = u8Data;
			oCommunicationManager.u16Counter++;
		break;
	case FSM_STATE_PLL:
		oCommunicationManager.u8PayloadLength = u8Data;
		if (oCommunicationManager.u8PayloadLength > 0) {
			oCommunicationManager.u8State = FSM_STATE_PL;
			oCommunicationManager.u8LastDataReceived[oCommunicationManager.u16Counter] = u8Data;
			oCommunicationManager.u16Counter++;
		}
		else {
			oCommunicationManager.u8State = FSM_STATE_CRC;
			oCommunicationManager.u8LastDataReceived[oCommunicationManager.u16Counter] = u8Data;
			oCommunicationManager.u16Counter++;
		}
		break;
	case FSM_STATE_PL:
		if (oCommunicationManager.u8PayloadLength > 0) {
			oCommunicationManager.u8PayloadLength--;
			oCommunicationManager.u8LastDataReceived[oCommunicationManager.u16Counter] = u8Data;
			oCommunicationManager.u16Counter++;
			if (oCommunicationManager.u8PayloadLength == 0) {
				oCommunicationManager.u8State = FSM_STATE_CRC;
			}
		}

		break;
	case FSM_STATE_CRC:
		if (oCommunicationManager.u8CounterCRC > 0) {
			oCommunicationManager.u8CounterCRC--;
			oCommunicationManager.u8LastDataReceived[oCommunicationManager.u16Counter] = u8Data;
			oCommunicationManager.u16Counter++;
			if (oCommunicationManager.u8CounterCRC == 0) {

				u16CalculatedCRC16 = CommunicationManagerCRC16(oCommunicationManager.u8LastDataReceived, oCommunicationManager.u16Counter);

				if (u16CalculatedCRC16 == 0x00) {
					//We found a valid data packet
					oCommunicationManager.u8State = FSM_STATE_HEADER;

					memcpy(pu8Data, oCommunicationManager.u8LastDataReceived, oCommunicationManager.u16Counter);
					*pu16Length = oCommunicationManager.u16Counter;
					return 1;
				}
				else {
					//It is invalid
					oCommunicationManager.u8State = FSM_STATE_HEADER;
					return 0;
				}
			}
		}
		break;
	}
	return 0;

}
/**
  * @brief  Check if message are contained inside a circular DMA buffer, the erase the message in the buffer
  * @param  pu8Data: Pointer to a size 255 uint8_t buffer
  * @param  pu16Length: Pointer to a uint16_t for the buffer length
  * @retval 1 if a new message has been received, 0 otherwise
  */
uint8_t CommunicationManagerProcessBuffer(uint8_t* pu8Data, uint16_t* pu16Length) {
	/*
	uint16_t u16Index;
	uint16_t u16CircularBufferIndex;
	uint16_t u16RegularIndex;
	uint16_t u16CalculatedCRC16;
	uint8_t u8Data = oCommunicationManager.u8DMABuffer[oCommunicationManager.u16Counter];

	if (pu8Data == NULL || pu16Length == NULL) {
		return 0;
	}

	switch (oCommunicationManager.u8State) {
	case FSM_STATE_HEADER:
		if (u8Data == START) {
			oCommunicationManager.u8State = FSM_STATE_PID;
			oCommunicationManager.u8CounterCRC = 2;
			oCommunicationManager.u8PayloadLength = 0;
			oCommunicationManager.u16StartLocation = oCommunicationManager.u16Counter;
		}
		break;
	case FSM_STATE_PID:
		if (u8Data == FSM_STATE_PID) {
			oCommunicationManager.u8State = FSM_STATE_CMD;
		}
		else {
			oCommunicationManager.u8State = FSM_STATE_HEADER;
			//We restart one char after the startLocation
			oCommunicationManager.u16Counter = ++oCommunicationManager.u16StartLocation % oCommunicationManager.u16DMABufferLength;
			return 0;
		}
		break;
	case FSM_STATE_CMD:
		oCommunicationManager.u8State = FSM_STATE_PLL;
		break;
	case FSM_STATE_PLL:
		oCommunicationManager.u8PayloadLength = u8Data;
		if (oCommunicationManager.u8PayloadLength > 0) {
			oCommunicationManager.u8State = FSM_STATE_PL;
		}
		else {
			oCommunicationManager.u8State = FSM_STATE_CRC;
		}
		break;
	case FSM_STATE_PL:
		if (oCommunicationManager.u8PayloadLength > 0) {
			oCommunicationManager.u8PayloadLength--;
			if (oCommunicationManager.u8PayloadLength == 0) {
				oCommunicationManager.u8State = FSM_STATE_CRC;
			}
		}

		break;
	case FSM_STATE_CRC:
		if (oCommunicationManager.u8CounterCRC > 0) {
			oCommunicationManager.u8CounterCRC--;
			if (oCommunicationManager.u8CounterCRC == 0) {
				//We have a complete message
				oCommunicationManager.u16EndLocation = oCommunicationManager.u16Counter++;

				if (oCommunicationManager.u16StartLocation > oCommunicationManager.u16EndLocation) {
					//Circular buffer copy
					u16Index = oCommunicationManager.u16StartLocation;
					u16RegularIndex = 0;
					while (u16Index % oCommunicationManager.u16DMABufferLength != (oCommunicationManager.u16EndLocation + 1) % oCommunicationManager.u16DMABufferLength) {
						u16CircularBufferIndex = u16Index % oCommunicationManager.u16DMABufferLength;
						oCommunicationManager.u8LastDataReceived[u16RegularIndex] = oCommunicationManager.u8DMABuffer[u16CircularBufferIndex];
						u16Index++;
						u16RegularIndex++;
					}
					oCommunicationManager.u16DataLength = u16RegularIndex;
				}
				else {
					//Regular buffer copy
					u16RegularIndex = 0;
					for (u16Index = oCommunicationManager.u16StartLocation; u16Index <= oCommunicationManager.u16EndLocation; u16Index++) {
						oCommunicationManager.u8LastDataReceived[u16RegularIndex] = oCommunicationManager.u8DMABuffer[u16Index];
						u16RegularIndex++;
					}
					oCommunicationManager.u16DataLength = u16RegularIndex;
				}

				u16CalculatedCRC16 = CommunicationManagerCRC16(oCommunicationManager.u8LastDataReceived, oCommunicationManager.u16DataLength);

				if (u16CalculatedCRC16 == 0x00) {
					//We found a valid data packet
					oCommunicationManager.u8State = FSM_STATE_HEADER;
					CommunicationManagerEraseBufferPart(oCommunicationManager.u16StartLocation, oCommunicationManager.u16EndLocation);

					memcpy(pu8Data, oCommunicationManager.u8LastDataReceived, oCommunicationManager.u16DataLength);
					*pu16Length = oCommunicationManager.u16DataLength;
					return 1;
				}
				else {
					//It is invalid
					oCommunicationManager.u8State = FSM_STATE_HEADER;
					oCommunicationManager.u16Counter = ++oCommunicationManager.u16StartLocation % oCommunicationManager.u16DMABufferLength;
					return 0;
				}
			}
		}
		break;
	}

	//We increment our counter while keeping it circular
	oCommunicationManager.u16Counter = ++oCommunicationManager.u16Counter % oCommunicationManager.u16DMABufferLength;
	*/
	return 0;
}

/**
  * @brief  Initialise the communication module
  */
void CommunicationManagerInit() {
	memset(&oCommunicationManager, 0, sizeof(oCommunicationManager));

	oCommunicationManager.u16Counter = 0;
	oCommunicationManager.u8PayloadLength = 0;
	oCommunicationManager.u8State = FSM_STATE_HEADER;
	oCommunicationManager.u8CounterCRC = 0;
	oCommunicationManager.u16DataLength = 0;
	oCommunicationManager.u8PID = 0;
	oCommunicationManager.u8ReceivedInit = 0;

	AES_init_ctx_iv(&oCommunicationManager.ctx, key2, key2);
}
