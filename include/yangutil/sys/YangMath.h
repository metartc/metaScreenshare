//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGMATH_H_
#define INCLUDE_YANGUTIL_SYS_YANGMATH_H_
#include <yangutil/yangtype.h>
static inline int16_t yang_rtp_seq_distance(const uint16_t prev_value, const uint16_t value)
{
    return (int16_t)(value - prev_value);
}
#ifdef __cplusplus
extern "C"{
#endif

int16_t yang_floattoint16(float f);
float yang_int16tofloat(int16_t i16);

uint64_t yang_random();
int32_t  yang_insert_uint16_sort(uint16_t a[],uint16_t value,uint32_t * alen);
int32_t  yang_insert_uint32_sort(uint32_t *a, uint32_t value, uint32_t *palen);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGMATH_H_ */
