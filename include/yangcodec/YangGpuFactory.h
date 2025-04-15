//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangGpuFactory_H
#define YangGpuFactory_H
#include <yangencoder/YangVideoEncoder.h>
#include <yangdecoder/YangVideoDecoder.h>
#include <yangutil/yangavinfotype.h>
class YangGpuFactory
{
public:

    YangGpuFactory();
    ~YangGpuFactory();
    YangVideoEncoder* createGpuEncoder();
    YangVideoDecoder* createGpuDecoder(YangVideoInfo* videoInfo);
};

#endif // YANGGPUENCODERFACTORY_H
