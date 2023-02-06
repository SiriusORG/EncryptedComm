/*
 *CommunicationManager.h
 *
 *  Created on: Sep 13, 2019
 *      Author: Alex Morin
 */

#ifndef CommunicationManager_H_
#define CommunicationManager_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "aes.h"

// A changer selon le VID du vehicule
#define START      0x16				// Le start byte

#include "datatypes.h"

	typedef struct {
		uint8_t u8LastDataReceived[256];
		uint8_t u8ReceivedInit;
		uint8_t u8State;
		uint8_t u8PayloadLength;
		uint8_t u8PID;

		uint16_t u16DataLength;
		uint16_t u16Counter;
		uint16_t u16DMABufferLength;
		uint16_t u8CounterCRC;
		struct AES_ctx ctx;
	} oCommunicationManager_t, *poCommunicationManager_t;

	uint8_t CommunicationManagerProcessBuffer(uint8_t* pu8Data, uint16_t* pu16Length);

	void CommunicationManagerInit();

	uint8_t CommunicationManagerProcessChar(uint8_t u8Data, uint8_t* pu8Data, uint16_t* pu16Length);

	void CommunicationManagerSendPacket(uint8_t u8CMD, uint8_t* pu8Payload, uint16_t u16PayloadLength);

#ifdef __cplusplus
}
#endif

#endif /*CommunicationManager_H_ */
