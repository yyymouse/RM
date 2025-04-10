#ifndef __DECODER_H
#define __DECODER_H

#include <stdint.h>

#include "protocol.h"

void decode_board_C(uint8_t* src, uint32_t len);
void decode_board_A(uint8_t* src, uint32_t len);
void decode_tof(uint8_t* src);

extern uint32_t tof_distance_1 ;
extern uint8_t data_buffer_transmit_c[128];
extern uint8_t data_buffer_A[128];
extern		uint8_t tof_rx_buffer_1[18];
#endif //__DECODER_H
