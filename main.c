#include <stdio.h>
#include <stdint.h> 
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "aes.h"
#include "communicationmanager.h"

uint8_t key[16] = { (uint8_t)0x2b, (uint8_t)0x7e, (uint8_t)0x15, (uint8_t)0x16, (uint8_t)0x28, (uint8_t)0xae, (uint8_t)0xd2, (uint8_t)0xa6, (uint8_t)0xab, (uint8_t)0xf7, (uint8_t)0x15, (uint8_t)0x88, (uint8_t)0x09, (uint8_t)0xcf, (uint8_t)0x4f, (uint8_t)0x3c };
uint8_t sample[] = { 0x97, 0x3F, 0xDC, 0x8C, 0xD5, 0x69, 0xB0, 0x1D, 0xEE, 0xCB, 0x3E, 0x89, 0x52, 0x18, 0x49, 0x49, 0x36, 0x41, 0xAE, 0xBA, 0x10, 0x24, 0x4D, 0xAA, 0x01, 0xAC, 0xD9, 0xEE, 0xAE, 0xF9, 0x6E, 0x15 };


static void phex(uint8_t* str, uint16_t u16Len) {
    unsigned char i;
    for (i = 0; i < u16Len; ++i)
        printf("0x%.2X ", str[i]);
    printf("\n");
}

void padBuffer(uint8_t u8BlockLen, uint8_t u8DataIn[], uint16_t u16DataInSize, uint8_t u8DataOut[], uint16_t* u16DataOutSize) {
    if (u16DataInSize % u8BlockLen == 0) {
        memcpy(u8DataOut, u8DataIn, u16DataInSize);
        *u16DataOutSize = u16DataInSize;
        return;
    }

    uint8_t numBytesToAdd = u8BlockLen - (u16DataInSize % u8BlockLen);

    memcpy(u8DataOut, u8DataIn, u16DataInSize);
    memset(&u8DataOut[u16DataInSize], numBytesToAdd, numBytesToAdd);
    *u16DataOutSize = u16DataInSize + numBytesToAdd;
}

void unpadBuffer(uint8_t u8DataIn[], uint16_t u16DataInSize, uint8_t u8DataOut[], uint16_t* u16DataOutSize) {
    uint8_t numBytesToRemove = u8DataIn[u16DataInSize - 1];

    bool bIsValid = true;
    for (int i = u16DataInSize - 1; i >= u16DataInSize - numBytesToRemove; i--) {
        if (numBytesToRemove != u8DataIn[i]) {
            bIsValid &= false;
            break;
        }
    }

    if (bIsValid) {
        memcpy(u8DataOut, u8DataIn, u16DataInSize - numBytesToRemove);
        *u16DataOutSize = u16DataInSize - numBytesToRemove;
    } else {
        // Right length, no padding removal needed
        memcpy(u8DataOut, u8DataIn, u16DataInSize);
        *u16DataOutSize = u16DataInSize;
    }
}

void generateIV(uint8_t u8IV[], uint8_t u8IVLen) {
    for (int i = 0; i < u8IVLen; i++) {
        u8IV[i] = rand() % 0xFF;
    }
}

void decryptSample(uint8_t u8SampleCipher[], uint8_t u8CipherLen) {
    uint8_t iv[AES_BLOCKLEN];
    uint8_t u8Buffer[64];
    uint8_t u8Data[16];
    uint16_t u16DataLen;

    memcpy(iv, u8SampleCipher, AES_BLOCKLEN);
    memcpy(u8Buffer, &u8SampleCipher[AES_BLOCKLEN], u8CipherLen - AES_BLOCKLEN);

    printf("    SAMPLE (%02d): ", u8CipherLen);
    phex(u8SampleCipher, u8CipherLen);
    printf("        IV (%02d): ", AES_BLOCKLEN);
    phex(iv, AES_BLOCKLEN);
    printf(" Encrypted (%02d): ", u8CipherLen - AES_BLOCKLEN);
    phex(u8Buffer, u8CipherLen - AES_BLOCKLEN);
    
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, u8Buffer, u8CipherLen - AES_BLOCKLEN);
    printf(" Decrypted (%02d): ", u8CipherLen - AES_BLOCKLEN);
    phex(u8Buffer, u8CipherLen - AES_BLOCKLEN);

    unpadBuffer(u8Buffer, u8CipherLen - AES_BLOCKLEN, u8Data, &u16DataLen);
    printf("  UNPADDED (%02d): ", u16DataLen);
    phex(u8Data, u16DataLen);
    printf("    RESULT (%02d): %s\r\n", u16DataLen, u8Data);
}

int main(int argc, char** argv) {

    printf("Testing packetization\\de-packetization\r\n");

    CommunicationManagerInit();
    uint8_t u8TestBuffer[] = { 0x16, 0x00, 0x23, 0x04, 0xDE, 0xAD, 0xBE, 0xEF, 0x3A, 0xF8 };
    uint8_t u8ReceivedData[0xFF];
    uint16_t u16Length;

    for (uint8_t i = 0; i < sizeof(u8TestBuffer); i++) {
        if (CommunicationManagerProcessChar(u8TestBuffer[i], u8ReceivedData, &u16Length)) {
            printf("Found Packet (%02d): ", u16Length);
            phex(u8ReceivedData, u16Length);
        };
    }

    for (uint8_t i = 0; i < sizeof(u8TestBuffer); i++) {
        if (CommunicationManagerProcessChar(u8TestBuffer[i], u8ReceivedData, &u16Length)) {
            printf("Found Packet (%02d): ", u16Length);
            phex(u8ReceivedData, u16Length);
        };
    }

    for (uint8_t i = 0; i < sizeof(u8TestBuffer); i++) {
        if (CommunicationManagerProcessChar(u8TestBuffer[i], u8ReceivedData, &u16Length)) {
            printf("Found Packet (%02d): ", u16Length);
            phex(u8ReceivedData, u16Length);
        };
    }

    uint8_t u8Data[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
    CommunicationManagerSendPacket(0x23, u8Data, 4);
    CommunicationManagerSendPacket(0x23, u8Data, 4);
    CommunicationManagerSendPacket(0x23, u8Data, 4);
    CommunicationManagerSendPacket(0x23, u8Data, 4);
    CommunicationManagerSendPacket(0x23, u8Data, 4);
    CommunicationManagerSendPacket(0x23, u8Data, 4);



    printf("Testing Sample Decryption\r\n");

    decryptSample(sample, sizeof(sample));


    printf("Testing Dummy Data encryption\\decryption\r\n");
    srand(time(0));

    uint8_t u8InBuffer[] = "ALEX";
    uint16_t u16InBufferSize = sizeof(u8InBuffer);
    uint8_t u8PaddedBuffer[256];
    uint16_t u16PaddedBufferSize;
    uint8_t u8ToSend[64];

    padBuffer(AES_BLOCKLEN, u8InBuffer, u16InBufferSize, u8PaddedBuffer, &u16PaddedBufferSize);

    printf("Padding buffer: \r\n");
    printf(" InBuffer (%02d): ", u16InBufferSize);
    phex(u8InBuffer, u16InBufferSize);
    printf("OutBuffer (%02d): ", u16PaddedBufferSize);
    phex(u8PaddedBuffer, u16PaddedBufferSize);


    struct AES_ctx ctx;

    uint8_t iv[AES_BLOCKLEN];
    generateIV(iv, AES_BLOCKLEN);

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, u8PaddedBuffer, u16PaddedBufferSize);
    printf("        IV (%02d): ", AES_BLOCKLEN);
    phex(iv, AES_BLOCKLEN);
    printf(" Encrypted (%02d): ", u16PaddedBufferSize);
    phex(u8PaddedBuffer, u16PaddedBufferSize);

    memcpy(u8ToSend, iv, AES_BLOCKLEN);
    memcpy(&u8ToSend[AES_BLOCKLEN], u8PaddedBuffer, u16PaddedBufferSize);

    printf("    ToSend (%02d): ", AES_BLOCKLEN + u16PaddedBufferSize);
    phex(u8ToSend, AES_BLOCKLEN + u16PaddedBufferSize);

    printf("Testing Full Packet path (Data -> Packet -> Encryption -> Decryption -> Packet -> Data)\r\n");

    printf("Press any key to exit...");
    (void)getchar();

    return 0;
}