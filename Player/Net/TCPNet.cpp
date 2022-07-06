#include "TCPNet.h"
#include "TCPKernel.h"

TCPNet::TCPNet(IKernel *kernel)
    :m_sockClient(NULL)
{
    m_TcpKernel = kernel;
}
TCPNet::~TCPNet()
{

}
//连接服务器
bool TCPNet::connectToServer(const char*IP,int port)
{
    WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
        wVersionRequested = MAKEWORD(2, 2);

        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0) {
            return false;
        }


        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
            disConnect();
            return false;
        }
            //QMessageBox::about(this,"提示","以达到添加的最大歌曲数量");;

        //LPWSTR
            //创建套接字
        m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        //连接服务器
        sockaddr_in serveraddr;
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(IP);
        serveraddr.sin_port = htons(port);
        if (connect(m_sockClient, (const sockaddr*)&serveraddr, sizeof(serveraddr)) == 0) {
            qDebug() << "connect success.............................\n";
        }

        return true;
}
//断开连接
void TCPNet::disConnect()
{
    WSACleanup();
    if(m_sockClient){
        closesocket(m_sockClient);
        m_sockClient = NULL;
    }

}
//接收数据
bool TCPNet::recvData()
{
    //循环接收数据 ：先接收包大小，再接收数据
    int packsize = 0;
    int offset = 0;
    int res = 0;
    char *pszbuf = nullptr;
    res = recvfrom(m_sockClient,(char*)&packsize,sizeof(packsize),0,nullptr,nullptr);
    if(res <= 0)
        return false;
    //为数据申请空间
    pszbuf = new char[packsize];
    //接收数据
    while(packsize){
        res = recvfrom(m_sockClient,pszbuf+offset,packsize,0,nullptr,nullptr);
        if(res < 0){
            //释放空间
            delete [] pszbuf;
            pszbuf = nullptr;
            return false;
        }

        packsize -= res;
        offset += res;
    }
    //交给核心处理类来处理
    m_TcpKernel->dealData(pszbuf);
    //释放空间
    delete [] pszbuf;
    pszbuf = nullptr;
    return true;
}
//发送数据
bool TCPNet::sendData(const char *msg,int nLen)
{
    //参数检验
    if(!msg || nLen <= 0)
        return false;
    //发送包大小
    if(!send(m_sockClient,(char*)&nLen,sizeof(nLen),0))
        return false;
    //发送数据包
    if(!send(m_sockClient,msg,nLen,0))
        return false;
    return true;
}

//获取客户端套接字
SOCKET TCPNet::GetClientSocket()
{
    return m_sockClient;
}
