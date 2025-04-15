//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGLIB_H_
#define INCLUDE_YANGUTIL_SYS_YANGLIB_H_

#include <yangutil/yangtype.h>

#if Yang_OS_WIN
#include <minwindef.h>
#endif

typedef struct{
	void* session;
	void* (*loadFunction)(void* session,const char *name);
}YangLib;

int32_t yang_create_lib(YangLib* lib,char* libPath);
void yang_destroy_lib(YangLib* lib);

#endif /* INCLUDE_YANGUTIL_SYS_YANGLIB_H_ */
