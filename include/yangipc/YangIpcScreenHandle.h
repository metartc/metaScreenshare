//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANGIPC2_YangIpcScreenHandle_H_
#define INCLUDE_YANGIPC2_YangIpcScreenHandle_H_

#include <yangutil/yangavinfo.h>
#include <yangipc/YangIpcConfig.h>

class YangIpcScreenHandle {
public:
	YangIpcScreenHandle(){};
	virtual ~YangIpcScreenHandle(){};
	virtual void init(YangIpcDataMsg* channelmsg)=0;

	virtual void start()=0;
	virtual yangbool isAlive()=0;
	static YangIpcScreenHandle* createIpcHandle(YangAVInfo* avinfo);

};

#endif /* INCLUDE_YANGIPC2_YANGIPCHANDLE_H_ */
