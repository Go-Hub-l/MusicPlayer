#include "INet.h"
#ifndef TCPNET_H
#define TCPNET_H

class IKernel;

class TCPNet:public INet{

public:
    TCPNet(IKernel *);
    virtual ~TCPNet();

public:
    virtual bool connectToServer(const char*SERVERIP,int port);
    virtual void disConnect();
    virtual bool recvData();
    virtual bool sendData(const char *msg,int nLen);
    SOCKET GetClientSocket();
private:
    SOCKET m_sockClient;//客户端套接字
    IKernel *m_TcpKernel;
};


#endif
