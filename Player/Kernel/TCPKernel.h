#ifndef TCPKERNEL_H
#define TCPKERNEL_H
#include "IKernel.h"
#include "../Net/TCPNet.h"
#include "packdef.h"
#include "videoplayer.h"
#include "mvwidget.h"

class TCPKernel;

//typedef char nPackdef;
typedef bool (TCPKernel::*PFUN)(const char*);

typedef struct {
    PackType m_nType;
    PFUN m_fun;
}protocolMapFunction;


class TCPKernel:public IKernel{
public:
    TCPKernel();
    virtual ~TCPKernel();

public:
    virtual bool connectToServer(const char* IP,int port);
    virtual bool sendData(const char* szbuf,int nLen);
    virtual bool recvData();
    virtual void dealData(const char* szbuf);

    bool RegisterRs(const char* szbuf);

    bool PlayMVRs(const char *szbuf);
    bool DownloadMV(const char *szbuf);
private:
    TCPNet *m_tcp;
    VideoPlayer *mPlayer;
};


#endif
