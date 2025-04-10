#ifndef __BSP_CRC_H
#define __BSP_CRC_H

#include <stdint.h>
#include <stdio.h>


uint8_t CRC8_calculate(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8);
uint16_t CRC16_calculate(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);

#endif //__BSP_CRC_H
