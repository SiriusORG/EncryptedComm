#include <stdio.h>
#include <stdint.h>
#include "crc.h"


int main() // Example of how to use the crc16() function in crc.c
{
    char *data = "123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789"; // gives a CRC-16/CCITT-FALSE checksum of 0xDB59
    printf("0x%04X\n", crc16(data)); // prints the CRC-16/CCITT-FALSE checksum

    return 0;
}
