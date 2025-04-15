//
// Copyright (c) 2019-2023 yanggaofeng
//
#include <yangencoder/YangGpuEncoderFactory.h>
#include <yangencoder/YangEncoderFactory.h>

#include <yangencoder/YangOpenH264Encoder.h>



YangEncoderFactory::YangEncoderFactory() {


}

YangEncoderFactory::~YangEncoderFactory() {

}



YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoCodec paet,YangVideoInfo *pcontext){

    if(pcontext->videoEncHwType==0){

        if (paet == Yang_VED_H264)
            return new YangOpenH264Encoder();


    }else{
#ifdef _WIN32
        YangGpuEncoderFactory gf;
        return gf.createGpuEncoder();
#else
        return NULL;
#endif
    }
    return NULL;
}

YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoInfo *pcontext){
    YangVideoCodec maet=Yang_VED_H264;
    if(pcontext->videoEncoderType==0) maet=Yang_VED_H264;
    if(pcontext->videoEncoderType==1) maet=Yang_VED_H265;

    return createVideoEncoder(maet,pcontext);
}
