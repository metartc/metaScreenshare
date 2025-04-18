﻿//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_
#define YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_
#include <yangutil/yangavinfotype.h>
#include "YangVideoEncoder.h"
#include "yangencoder/YangAudioEncoder.h"
#include <yangavutil/video/YangVideoEncoderMeta.h>


class YangEncoderFactory {
public:
	YangEncoderFactory();
	virtual ~YangEncoderFactory();
	
	YangVideoEncoder* createVideoEncoder(YangVideoCodec paet,YangVideoInfo *pcontext);
	YangVideoEncoder* createVideoEncoder(YangVideoInfo *pcontext);
	
};

#endif /* YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_ */
