/***********************************************************************
* @ 客户端IOCP
* @ brief
	1、将Net/client中的文件另起工程，即可测试
	2、对比server端的代码结构，加深理解
* @ author zhoumf
* @ date 2016-7-19
************************************************************************/
#pragma  once
//////////////////////////////////////////////////////////////////////////
// 使用winsock2  避免同winsock冲突
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
//////////////////////////////////////////////////////////////////////////
#include <mswsock.h>
#include "tool\cLock.h"
#include "tool\buffer.h"
#include "config_client.h"

class ClientLink;

typedef void(*HandleMsgFunc)(void* pMsg, DWORD size);

enum EnumIO{ IO_Write, IO_Read };

struct My_OVERLAPPED : public OVERLAPPED
{
	ClientLink*	client;
	EnumIO		eType;

	void SetLink(ClientLink* p)
	{
		memset((char *)this, 0, sizeof(OVERLAPPED));
		client = p;
	}
};

class ClientLink{
	enum EStatus { State_Close, State_Connecting, State_Connected };
public:
	ClientLink(const ClientLinkConfig& info);
    //~ClientLink(){};

	static bool InitWinsock();
	static bool CleanWinsock();

    bool CreateLinkAndConnect(HandleMsgFunc handleMsg);
    void CloseClient(int nErrorCode);
    void SendMsg(void* pMsg, DWORD size);
    bool IsConnect(){ return _eState == State_Connected; }

private:
    static void CALLBACK DoneIO(DWORD, DWORD, LPOVERLAPPED);
    void DoneIOCallback(DWORD dwNumberOfBytesTransferred, EnumIO eFlag);

    BOOL ConnectEx();
	void OnConnect();

	bool PostSend(char* buffer, DWORD nLen);
	bool PostRecv(char* buffer, DWORD nLen);

	void OnSend_DoneIO(DWORD dwBytesTransferred);
	void OnRead_DoneIO(DWORD dwBytesTransferred);
    void RecvMsg(char* pMsg, DWORD size);
private:
	EStatus _eState = State_Close;
	SOCKET _sClient = INVALID_SOCKET;
	My_OVERLAPPED _ovRecv;
	My_OVERLAPPED _ovSend;

	net::Buffer _recvBuf;
	net::Buffer _sendBuf;

	bool _bCanWrite = true;

    cMutex _csRead;
    cMutex _csWrite;

	const ClientLinkConfig& _config;

    void*               _player = NULL;
    HandleMsgFunc       _HandleServerMsg = NULL;
};
