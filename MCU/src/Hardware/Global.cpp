#include "Global.h"

uint16_t *GlobalParams::Status[6] = {
    &(GlobalParams::Status_Word_1.ToUint16),
    &(GlobalParams::Status_Word_2.ToUint16),
    &(GlobalParams::Status_Word_3.ToUint16),
    &(GlobalParams::Status_Word_4.ToUint16),
    &(GlobalParams::Status_Word_5.ToUint16),
    &(GlobalParams::Status_Word_6.ToUint16)};

Status_Word_1_T GlobalParams::Status_Word_1 = {0};
Status_Word_2_T GlobalParams::Status_Word_2 = {0};
Status_Word_3_T GlobalParams::Status_Word_3 = {0};
Status_Word_4_T GlobalParams::Status_Word_4 = {0};
Status_Word_5_T GlobalParams::Status_Word_5 = {0};
Status_Word_6_T GlobalParams::Status_Word_6 = {0};
