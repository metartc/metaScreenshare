//
// Copyright (c) 2019-2023 yanggaofeng
//


#include <yangcapture/YangCaptureFactory.h>
#include <yangcapture/YangScreenCaptureImpl.h>


YangCaptureFactory::YangCaptureFactory() {


}

YangCaptureFactory::~YangCaptureFactory() {

}

YangScreenCapture *YangCaptureFactory::createScreenCapture(YangContext *pcontext){
	return new YangScreenCaptureImpl(pcontext);
}
