//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_BUFFER2_YANGPACKET_H_
#define INCLUDE_YANGUTIL_BUFFER2_YANGPACKET_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>

typedef struct{
	uint16_t baseSn;
	uint16_t endSn;
	uint16_t length;
	uint16_t capacity;
	uint64_t timestamp;
	uint8_t* payload;
}YangFecPacket;

typedef struct{
	uint16_t putIndex;
	uint16_t getIndex;
	int16_t  vsize;
	uint16_t capacity;
	YangFecPacket* packetData;
}YangFecPacketBuffer;


typedef struct{
	uint16_t putIndex;
	uint16_t getIndex;
	int16_t  vsize;
	uint16_t capacity;
	YangPacket* packetData;
}YangPacketBuffer;

typedef struct{
	uint16_t putIndex;
	uint16_t getIndex;
	int16_t  vsize;
	uint16_t capacity;
	uint32_t bytes;
	YangPacket** packetData;
}YangPacketList;

void yang_pkt_pushList(YangPacketList* pushList,YangPacket* pkt);
void yang_pkt_readList(YangPacketList* pushList,uint32_t nb);
uint32_t yang_pkt_getPacketCount(YangPacketList* pushList,uint32_t nb);

YangPacket* yang_pkt_getPutPacket(YangPacketBuffer* dataBuffer);
YangPacket* yang_pkt_getPacketBySeq(YangPacketBuffer* dataBuffer,uint16_t seq);
int32_t yang_pkt_checkBuffer(YangPacketBuffer* dataBuffer,uint32_t capacity);

YangPacketBuffer* yang_create_packetBuffer(int32_t rtpCount,int32_t payloadSize);
void yang_destroy_packetBuffer(YangPacketBuffer* pushDataBuffer);

YangFecPacket* yang_pkt_getFecPutPacket(YangFecPacketBuffer* dataBuffer);

YangFecPacketBuffer* yang_create_fecPacketBuffer(int32_t rtpCount,int32_t payloadSize);
void yang_destroy_fecPacketBuffer(YangFecPacketBuffer* pushDataBuffer);

YangPacketList* yang_create_packetList(int32_t rtpCount);

#endif /* INCLUDE_YANGUTIL_BUFFER2_YANGPACKET_H_ */
