#include <iostream>

#define _AES_H_

#include <stdint.h>
#include <stddef.h>

// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// CBC enables AES encryption in CBC-mode of operation.
// CTR enables encryption in counter-mode.
// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously.

#define CBC 1
#define CTR 0
#define ECB 0

#define AES128 1
//#define AES192 1
//#define AES256 1

#define AES_BLOCKLEN 16 // Block length in bytes - AES is 128b block only

#if defined(AES256) && (AES256 == 1)
    #define AES_KEYLEN 32
    #define AES_keyExpSize 240
#elif defined(AES192) && (AES192 == 1)
    #define AES_KEYLEN 24
    #define AES_keyExpSize 208
#else
    #define AES_KEYLEN 16   // Key length in bytes
    #define AES_keyExpSize 176
#endif

struct AES_ctx
{
  uint8_t RoundKey[AES_keyExpSize];
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
  uint8_t Iv[AES_BLOCKLEN];
#endif
};

void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key);
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
void AES_init_ctx_iv(struct AES_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void AES_ctx_set_iv(struct AES_ctx* ctx, const uint8_t* iv);
#endif

#if defined(ECB) && (ECB == 1)
// buffer size is exactly AES_BLOCKLEN bytes; 
// you need only AES_init_ctx as IV is not used in ECB 
// NB: ECB is considered insecure for most uses
void AES_ECB_encrypt(const struct AES_ctx* ctx, uint8_t* buf);
void AES_ECB_decrypt(const struct AES_ctx* ctx, uint8_t* buf);

#endif // #if defined(ECB) && (ECB == !)


#if defined(CBC) && (CBC == 1)
// buffer size MUST be mutile of AES_BLOCKLEN;
// Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
// NOTES: you need to set IV in ctx via AES_init_ctx_iv() or AES_ctx_set_iv()
//        no IV should ever be reused with the same key 
void AES_CBC_encrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, size_t length);
void AES_CBC_decrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, size_t length);

#endif // #if defined(CBC) && (CBC == 1)


#if defined(CTR) && (CTR == 1)

// Same function for encrypting as for decrypting. 
// IV is incremented for every block, and used after encryption as XOR-compliment for output
// Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
// NOTES: you need to set IV in ctx with AES_init_ctx_iv() or AES_ctx_set_iv()
//        no IV should ever be reused with the same key 
void AES_CTR_xcrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, size_t length);

#endif // #if defined(CTR) && (CTR == 1)

//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================

#define INC_DATATYPES_H_

/*#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif*/


#include "stdbool.h"
#include "stdint.h"



/*
typedef struct __attribute__((__packed__)) {
    float fLongitude;
    float fLatitude;
    float fMSLAlt;
    uint8_t u8Fix;
    uint8_t u8NumSat;
} oCAMM8QData_t, * poCAMM8QData_t;

typedef struct __attribute__((__packed__)) {
    float fTemperature;
    float fHumidity;
    float fPressure;
} oBME280Data_t, * poBME280Data_t;

typedef struct __attribute__((__packed__)) {
    oBNO055Data_t BNO055Data;
    oCAMM8QData_t CAMM8QData;
    oBME280Data_t BME280Data;
} oRawData_t, * poRawData_t;

typedef struct __attribute__((__packed__)) {
    float fBearing;
    float fSpeed;
    float fTargetDistance;
    float fAltitude;
    float fStartPressure;
    uint16_t u16LeftSpeed;
    uint8_t u8LeftDirection;
    uint16_t u16RightSpeed;
    uint8_t u8RightDirection;
    uint32_t u32Time;
    uint32_t u32Battery;				// Battery voltage in milivolt
    uint8_t u8Mode;
    uint8_t u8EStop;
} oVehiculeData_t, * poVehiculeData_t;

typedef struct __attribute__((__packed__)) {
    float fReleaseAltitude;                     //Release altitude in m
    float fArmingAltitude;                      //Arming altitude in m
    float fHardReleaseAltitude;                 //First drop altitude in m in case of issue
    float fSoftReleaseAltitude;                 //Second drop altitude in m, for ground offset error
    float fTargetRadius;                        //Max distance to be considered on target in m
    float fTargetLat;                           //Target latitude
    float fTargetLong;                          //Target longitude

    uint16_t u16HardReleaseCountdown;           //Time before executing a hard realease in ms after reaching fHardReleaseAltitude
    uint16_t u16SoftReleaseCountdown;           //Time before executing a soft realease in ms after reaching fSoftReleaseAltitude
    uint16_t u16MaxSpeed;                       //Max allowed motor speed between 0 - 4096
    uint16_t u16HeartbeatTransmitInterval;      //Interval between hearbeat transmission in ms
    uint16_t u16SensorTransmitInterval;         //Interval between sensor transmission in ms
    uint8_t u8VID;                              //Vehicule ID between 0 - 255
    bool bSound;                                //Enables or disables sound, true = enabled
    bool bExternalTelemetry;                    //Uses external telemetry, true = external
} oConfig_t, * poConfig_t;

typedef struct __attribute__((__packed__)) {
    uint32_t u32Timestamp;
} oHeartbeatData_t, * poHeartbeatData_t;

typedef struct __attribute__((__packed__)) {
    uint16_t u16LeftSpeed;
    uint8_t u8LeftDirection;
    uint16_t u16RightSpeed;
    uint8_t u8RightDirection;
} oMotorSpeedData_t, * poMotorSpeedData_t;
*/


//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================



#define CommunicationManager_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

// A changer selon le VID du vehicule
#define START      0x16				// Le start byte


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

//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
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
	//printf("     Sending (%02d): ", 6 + u16PayloadLength);

	unsigned char i;
	//for (i = 0; i < 6 + u16PayloadLength; ++i)
	//	printf("0x%.2X ", u8Reply[i]);
	//printf("\n");
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

	//AES_init_ctx_iv(&oCommunicationManager.ctx, key2, key2);
}

//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================


int main()
{
    printf("Testing packetization\\de-packetization\r\n");
    CommunicationManagerInit();
    printf("- Communication Manager Init Done\r\n");
    uint8_t u8TestBuffer[] = { 0x16, 0x00, 0x23, 0x04, 0xDE, 0xAD, 0xBE, 0xEF, 0x3A, 0xF8 };
    uint8_t u8ReceivedData[0xFF];
    uint16_t u16Length;
    printf("- Creating fake packet... ");
    for (uint8_t i = 0; i < sizeof(u8TestBuffer); i++) {
        if (CommunicationManagerProcessChar(u8TestBuffer[i], u8ReceivedData, &u16Length)) {
            printf("Found Packet (%02d): ", u16Length);
            //phex(u8ReceivedData, u16Length);
        }
    }; 

    printf("Done\r\n");
    

}
