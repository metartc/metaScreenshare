//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef SRC_YANGP2P_YANGIPC_H_
#define SRC_YANGP2P_YANGIPC_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>

#include <yangipc/YangIpcConfig.h>
#include <yangipc/YangIpcEncoder.h>
#include <yangsignal/YangIpcMessageDef.h>
#include <yangsignal/YangIpcMessageServer.h>

#include "YangIpcRtc.h"

class YangIpc{
public:
	YangIpc(YangIpcConfig* config);
	virtual ~YangIpc();
	int32_t  init();
	int32_t  start();
	int32_t  stop();

	void     checkAlive();
	yangbool isAlive();
	int32_t  getIpcHandle(YangIpcHandle *handle);
public:
	yangbool m_enableP2p;
	yangbool m_isInitRtc;

	YangAVInfo* m_avinfo;
	YangIpcDataMsg* m_dataMsg;

	YangIpcRtc* m_rtc;
	YangEncoderVideo* m_videoEncoder;
	YangIpcMessageServer* m_mqttServer;

	YangIpcDataCallback m_dataCallback;
	YangRtcCallback m_rtcCallback;
	YangIpcHandle m_ipcHandle;
};



#endif /* SRC_YANGP2P_YANGIPC_H_ */
