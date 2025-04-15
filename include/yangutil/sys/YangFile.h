//
// Copyright (c) 2019-2024 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGFILE_H_
#define INCLUDE_YANGUTIL_SYS_YANGFILE_H_
#include <yangutil/yangtype.h>
#ifdef __cplusplus
extern "C"{
#endif
int32_t  yang_getCurpath(char* path);
int32_t  yang_getLibpath(char* path);
int32_t  yang_getCaFile(char* pem,char* key);
int32_t  yang_getServerCaFile(char* filename,char* crt,char* key);
int32_t  yang_getServerCaFromFile(char *filename, char **buffer);
int32_t  yang_loadFileToStr(char *filename, char **str, int32_t  *len);
yangbool yang_getFileExists(char* path);
int32_t yang_createDir(char* dir);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGFILE_H_ */
