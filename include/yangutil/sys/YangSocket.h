//
// Copyright (c) 2019-2024 yanggaofeng
//
#ifndef YANGSOCKETUTIL_H_
#define YANGSOCKETUTIL_H_
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangVector.h>

#if Yang_OS_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <errno.h>
#define socklen_t int
#define yang_be32toh ntohl
#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)
#define YANG_NO_SIGNAL 0
#define yang_poll WSAPoll
#define YANG_EWOULDBLOCK WSAEWOULDBLOCK
#else
#include <netinet/in.h>
#include <errno.h>
	#if Yang_OS_APPLE
		#include <TargetConditionals.h>
        #include <libkern/OSByteOrder.h>
        #define yang_be32toh OSSwapBigToHostInt32
		#define YANG_NO_SIGNAL SO_NOSIGPIPE
	#else
        #define yang_be32toh be32toh
		#define YANG_NO_SIGNAL MSG_NOSIGNAL
	#endif

#define yang_poll poll
#define GetSockError()	errno
#define SetSockError(e)	errno = e
#define YANG_EWOULDBLOCK EWOULDBLOCK
#endif

#define yang_inet_pton inet_pton
#define yang_inet_addr inet_addr
#define yang_inet_ntoa inet_ntoa

#define yang_swap16(x) ((((uint16_t)(x) & 0xff00) >> 8) | (((uint16_t)(x) & 0x00ff) << 8))
#define yang_swap32(x) ((((uint32_t )(x) & 0xff000000) >> 24) | \
				   (((uint32_t )(x) & 0x00ff0000) >>  8) | \
				   (((uint32_t )(x) & 0x0000ff00) <<  8) | \
				   (((uint32_t )(x) & 0x000000ff) << 24))



#define yang_socket_t int32_t 
typedef struct sockaddr_in yang_socket_addr4;
typedef struct sockaddr_in6 yang_socket_addr6;


#if Yang_Enable_Sysbyteswap

#define yang_htons htons
#define yang_htonl htonl
#define yang_ntohl ntohl
#define yang_ntohs ntohs
#else
uint16_t yang_htons(uint16_t hs);
uint32_t  yang_htonl(uint32_t  hl);
uint16_t yang_ntohs(uint16_t ns);
uint32_t  yang_ntohl(uint32_t  nl);
#endif

typedef struct {
    YangIpFamilyType familyType;
    yang_socket_addr4 addr4;
    yang_socket_addr6 addr6;

} YangIpAddress;

yang_vector_declare(YangIpAddress)

#ifdef __cplusplus
extern "C"{
#endif

void yang_addr_set(YangIpAddress* addr,char* ip,int32_t  port,YangIpFamilyType familyType);
void yang_addr_setAnyAddr(YangIpAddress* addr,int32_t  port,YangIpFamilyType familyType);
void yang_addr_updatePort(YangIpAddress* addr,int32_t  port);
void yang_addr_copy(YangIpAddress* srcAddr,YangIpAddress* dstAddr,YangIpFamilyType familyType);

yangbool yang_addr_cmp(YangIpAddress* srcAddr,YangIpAddress* dstAddr);

void yang_addr_setIPV4(YangIpAddress* addr,int32_t  ip,int32_t  port);
void yang_addr_setIPV6(YangIpAddress* addr,uint8_t ip[16],int32_t  port);
uint32_t  yang_addr_getIP(YangIpAddress* addr);
uint8_t* yang_addr_getV6IP(YangIpAddress* addr);
void yang_addr_getIPStr(YangIpAddress* addr,char* addrstr,int32_t  strLen);
uint16_t yang_addr_getPort(YangIpAddress* addr);
uint16_t yang_addr_getSinPort(YangIpAddress* addr);
yang_socket_t yang_socket_create(YangIpFamilyType familyType, YangSocketProtocol protocol);
yang_socket_t yang_socket_create2(YangIpFamilyType familyType, YangSocketProtocol protocol,int32_t  timeoutSecond);
//non-block socket
int32_t  yang_socket_setNonblock(yang_socket_t fd);
int32_t  yang_socket_close(yang_socket_t fd);

int32_t  yang_socket_listen(yang_socket_t fd,YangIpAddress* addr,YangSocketProtocol protocol);

int32_t  yang_socket_connect(yang_socket_t fd,YangIpAddress* remoteAddr);
int32_t  yang_socket_recvfrom(yang_socket_t fd,char* buffer,int32_t  bufferLen,YangIpAddress* addr);

int32_t  yang_socket_accept(yang_socket_t fd,YangIpAddress* addr);

int32_t  yang_socket_sendto(yang_socket_t fd,char* data,int32_t  nb,YangIpAddress* remote_addr,int32_t  flag);
int32_t  yang_socket_send(yang_socket_t fd,char* data,int32_t  nb);
int32_t  yang_socket_send2(yang_socket_t fd,char* data,int32_t  nb,int32_t  flag);
int32_t  yang_socket_recv(yang_socket_t fd,char* data,int32_t  nb,int32_t  flag);


int32_t  yang_getLocalInfo(YangIpFamilyType familyType,char* p);
int32_t  yang_getLocalInfoList(YangIpFamilyType familyType,YangStringVector* p);
int32_t  yang_getIp(YangIpFamilyType familyType, char* domain, char* ip);

#ifdef __cplusplus
}
#endif
#endif /* YANGSOCKETUTIL_H_ */
