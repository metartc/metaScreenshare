//
// Copyright (c) 2019-2024 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANG_UNISTD_H_
#define INCLUDE_YANGUTIL_YANG_UNISTD_H_
#include <yang_config.h>
#ifdef __cplusplus
extern "C" {
#endif
	void yang_usleep_win(uint32_t milli_seconds);
	void yang_sleep_us(uint32_t micro_seconds);
	void yang_sleep_ms(uint32_t milli_seconds);

#ifdef __cplusplus
}
#endif

#ifdef _WIN32
#include <Windows.h>
#define yang_sleep(x) Sleep(1000*x)
//#define yang_sleep(x) SleepEx(1000*x,TRUE)

#define yang_exit ExitProcess
#else
#include <unistd.h>
#define yang_sleep sleep
#define yang_exit _exit
#endif

#if Yang_Enable_USLEEP
#ifdef _WIN32
#define yang_usleep yang_usleep_win
#else
#define yang_usleep usleep
#endif
#else
#define yang_usleep yang_sleep_us
#endif
#endif /* INCLUDE_YANGUTIL_YANG_UNISTD_H_ */
