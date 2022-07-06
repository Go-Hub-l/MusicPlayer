#ifndef INET_H
#define INET_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <QDebug>
#include <QMessageBox>

class INet{

public:
    INet(){}
    virtual ~INet(){}

public:
    virtual bool connectToServer(const char*SERVERIP,int port) = 0;
    virtual void disConnect() = 0;
    virtual bool recvData() = 0;
    virtual bool sendData(const char *msg,int nLen) = 0;
};


#endif
