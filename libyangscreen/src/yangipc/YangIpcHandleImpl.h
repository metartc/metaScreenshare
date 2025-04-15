//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANGIPC2_YangIpcHandleImpl_H_
#define INCLUDE_YANGIPC2_YangIpcHandleImpl_H_

#include <yangutil/yangavinfo.h>
#include <yangutil/sys/YangCTimer.h>
#include <yangipc/YangIpcEncoder.h>
#include <yangipc/YangIpcScreenHandle.h>
#include "YangIpc.h"
class YangIpcHandleImpl:public YangIpcScreenHandle {
public:
	YangIpcHandleImpl(YangAVInfo* avinfo);
	virtual ~YangIpcHandleImpl();
	void init(YangIpcDataMsg* channelmsg);

	void start();
	yangbool isAlive();

	YangIpc *m_ipc;
private:
	yangbool m_isInit;
	YangCTimer *m_timer_5s;
	YangAVInfo *m_avinfo;
	YangEncoderVideo *m_videoEncoder;

private:
	void initCtimer();
	void stopCtimer();

	yangbool encoder_enable();
};

#endif /* INCLUDE_YANGIPC2_YangIpcHandleImpl_H_ */
