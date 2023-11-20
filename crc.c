#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "crc.h"

unsigned short int crc16(char *p) // Computes CRC-16/CCITT-FALSE checksum of input array
{
    int n = (unsigned long)strlen(p);
    unsigned short int crc = 0xffff;

    while (n-- > 0) {
        crc  = (unsigned char)(crc >> 8) | (crc << 8);
        crc ^= *p++;
        crc ^= (unsigned char)(crc & 0xff) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xff) << 4) << 1;
    }

    return(crc);
}
