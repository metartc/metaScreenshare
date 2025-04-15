//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_
#define INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_


#include "YangScreenCapture.h"
#include <yangutil/yangavinfotype.h>

class YangCaptureFactory {
public:
	YangCaptureFactory();
	virtual ~YangCaptureFactory();

	YangScreenCapture *createScreenCapture(YangContext *pcontext);

};

#endif /* INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_ */
