#ifndef IKERNEL_H
#define IKERNEL_H

class IKernel{
public:
    IKernel(){}
    virtual ~IKernel(){}

public:
    virtual bool connectToServer(const char* SERVERIP,int port) = 0;
    virtual bool sendData(const char* szbuf,int nLen) = 0;
    virtual bool recvData() = 0;
    virtual void dealData(const char* szbuf) = 0;
};


#endif
