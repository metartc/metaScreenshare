//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGUSERTOKEN_H
#define YANGUSERTOKEN_H

#include <yangutil/yangtype.h>
#ifdef _MSC_VER
#include <Windows.h>
class YangUsertoken
{
public:
    YangUsertoken(bool isServiceProcess);
    ~YangUsertoken();
    void init();
private:
     HANDLE h;
};
#endif
#endif // YANGUSERTOKEN_H
