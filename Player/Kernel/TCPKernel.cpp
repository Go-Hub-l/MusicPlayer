#include "TCPKernel.h"
#include <QFile>
//消息映射表
protocolMapFunction m_protocolMapFunction[] = {
    {DEF_PACK_REGISTER_RS,&TCPKernel::RegisterRs},
    {DEF_PACK_PLAY_MV_RS,&TCPKernel::PlayMVRs},
    {DEF_PACK_DOWNLOAD_MV_RS,&TCPKernel::DownloadMV},
    {0,0}
};



TCPKernel::TCPKernel()
{
    m_tcp = new TCPNet(this);
    //播放视频
    mPlayer = new VideoPlayer;
}
TCPKernel::~TCPKernel()
{
    if(m_tcp){
        delete m_tcp;
        m_tcp = nullptr;
    }
    //释放MV
    if(mPlayer){
        delete mPlayer;
        mPlayer = nullptr;
    }
}

bool TCPKernel::connectToServer(const char* IP,int port)
{
    return m_tcp->connectToServer(IP,port);
}

bool TCPKernel::sendData(const char* szbuf,int nLen)
{
    return m_tcp->sendData(szbuf,nLen);
}
bool TCPKernel::recvData()
{
    return m_tcp->recvData();
}
//从客户端下载文件
bool TCPKernel::DownloadMV(const char *szbuf)
{
    //给服务器发送ACK确认
    send(m_tcp->GetClientSocket(),"1",2,0);
    qDebug()<<"start recv message";
    STRU_DOWNLOAD_MV_RS *sdmr = (STRU_DOWNLOAD_MV_RS*)szbuf;
    //后面改进为多线程下载文件!!!
    //将客户端传过来的数据保存到文件中
    QFile file(QString("./MV/%1").arg(sdmr->m_szMVName));
    //QFile file(QString("./MV/111.mp4"));
    char file_path[MAX_PATH] = {0};
    sprintf(file_path,"open file %s failed",sdmr->m_szMVName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append)){
        //qDebug()<<"open file"<<spmr->m_szMVName<<" failed";
        file.write(file_path, qstrlen(file_path));
        //perror(file_path);
        return false;
    }
    char buf[MAX_CONTENT_LEN] = {0};
    //long long dataCount = 0;
    //循环接收服务端数据
    while(sdmr->m_nPackSize){
        int rsize = -1;
        int wsize = -1;
        rsize = recvfrom(m_tcp->GetClientSocket(),buf,MAX_CONTENT_LEN,0,nullptr,nullptr);
        if(rsize < 0){
            qDebug()<<"recvform "<<sdmr->m_szMVName<<" failed";
            return false;
        }
        //将数据写入文件
        wsize = file.write(buf,rsize);//读多少写多少
        if(wsize < 0){
            qDebug()<<"write file"<<sdmr->m_szMVName<<" failed";
            return false;
        }
        sdmr->m_nPackSize -= rsize;
        memset(buf,0,MAX_CONTENT_LEN);//清空缓冲区，准备接收下一轮数据
        //给服务器发送ACK确认
        send(m_tcp->GetClientSocket(),"1",2,0);
    }
    qDebug()<<"recv message finised";
    file.close();
    return true;
}

void TCPKernel::dealData(const char* szbuf)
{
    //消息映射表
    PackType type = *(PackType*)(szbuf);
    int index = 0;
    while(1){
        if(type == m_protocolMapFunction[index].m_nType){
            (this->*m_protocolMapFunction[index].m_fun)(szbuf);
            break;
        }
        else if (m_protocolMapFunction[index].m_nType == 0 && m_protocolMapFunction[index].m_fun == 0)
            break;
        ++index;
    }
}
//注册回复
bool TCPKernel::RegisterRs(const char *szbuf)
{


}
//MV回复
bool TCPKernel::PlayMVRs(const char *szbuf)
{
    //STRU_PLAY_MV_RS *spmr = (STRU_PLAY_MV_RS*)szbuf;
     //DownloadMV(szbuf);
    //播放
    //mPlayer->setFileName(spmr->m_szMVName);
    //mPlayer->startPlay();

    return true;
}
