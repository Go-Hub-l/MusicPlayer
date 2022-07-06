#include<TCPKernel.h>
#include "packdef.h"
#include<stdio.h>

using namespace std;


////注册
//#define  DEF_PACK_REGISTER_RQ    (DEF_PACK_BASE + 0)
//#define  DEF_PACK_REGISTER_RS    (DEF_PACK_BASE + 1)
////登录
//#define  DEF_PACK_LOGIN_RQ    (DEF_PACK_BASE + 2)
//#define  DEF_PACK_LOGIN_RS    (DEF_PACK_BASE + 3)
//协议映射表
static const ProtocolMap m_ProtocolMapEntries[] =
{
    {DEF_PACK_REGISTER_RQ , &TcpKernel::RegisterRq},
    {DEF_PACK_LOGIN_RQ , &TcpKernel::LoginRq},
    {DEF_PACK_PLAY_MV_RQ,&TcpKernel::PlayMV},
    {DEF_PACK_DOWNLOAD_MV_RQ,&TcpKernel::DownloadMV},
    {0,0}
};
#define RootPath   "/home/colin/Video/"

int TcpKernel::Open()
{
    m_sql = new CMysql;
    m_tcp = new TcpNet(this);
    m_tcp->SetpThis(m_tcp);
    pthread_mutex_init(&m_tcp->alock,NULL);
    pthread_mutex_init(&m_tcp->rlock,NULL);
    if(  !m_sql->ConnectMysql("localhost","root","123456","music")  )
    {
        printf("Conncet Mysql Failed...\n");
        return FALSE;
    }
    else
    {
        printf("MySql Connect Success...\n");
    }
    if( !m_tcp->InitNetWork()  )
    {
        printf("InitNetWork Failed...\n");
        return FALSE;
    }
    else
    {
        printf("Init Net Success...\n");
    }

    return TRUE;
}

void TcpKernel::Close()
{
    m_sql->DisConnect();
    m_tcp->UnInitNetWork();
}


void TcpKernel::DealData(int clientfd,char *szbuf,int nlen)
{
    PackType *pType = (PackType*)szbuf;
    int i = 0;
    while(1)
    {
        if(*pType == m_ProtocolMapEntries[i].m_type)
        {
            PFUN fun= m_ProtocolMapEntries[i].m_pfun;
            (this->*fun)(clientfd,szbuf,nlen);
        }
        else if(m_ProtocolMapEntries[i].m_type == 0 &&
                m_ProtocolMapEntries[i].m_pfun == 0)
            return;
        ++i;
    }
    return;
}


//注册
void TcpKernel::RegisterRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d RegisterRq\n", clientfd);

    STRU_REGISTER_RQ * rq = (STRU_REGISTER_RQ *)szbuf;
    STRU_REGISTER_RS rs;

    m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
}
//登录
void TcpKernel::LoginRq(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d LoginRq\n", clientfd);

    STRU_LOGIN_RQ * rq = (STRU_LOGIN_RQ *)szbuf;
    STRU_LOGIN_RS rs;

    m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
}
//MV下载请求
void TcpKernel::DownloadMV(int clientfd,char* szbuf,int nlen)
{
    STRU_DOWNLOAD_MV_RQ *sdmr = (STRU_DOWNLOAD_MV_RQ*)szbuf;

    STRU_DOWNLOAD_MV_RS rs;
    strcpy(rs.m_szMVName,sdmr->m_szMVName);
    //打开文件
    char file_path[MAX_PATH]={0};
    char block[2]={0};//接收客户端发来的确认号
    sprintf(file_path,"../MV/%s",sdmr->m_szMVName);
    int fd = open(file_path,O_RDONLY);
    if(fd < 0){
        perror("TcpKernel::PlayMV open file failed");
        return ;
    }
    //计算文件大小
    rs.m_nPackSize = lseek(fd,0,SEEK_END);
    //计算所需线程数

    //给客户端发送回复：可读端准备接收数据消息
    m_tcp->SendData(clientfd,(char *)&rs,sizeof(rs));
    //接收服务器发来的确认号
    if(recv(clientfd,block,1,0) == -1){
        perror("recv client request failed");
        return;
    }
    //重新移动到开头  读取数据
    lseek(fd,0,SEEK_SET);
    //循环从文件中读，发送
    //char buf[DEF_BUFF_SIZE]={0};
    int rSize = -1;
    char buf[DEF_BUFF_SIZE] = {0};
    printf("TcpKernel::PlayMV start send message\n");
    while((rSize = read(fd,buf,DEF_BUFF_SIZE)) > 0){
        //给客户端发送读取到的数据
        if(send(clientfd,buf,rSize,0) == -1){
            perror("TCPKernel::DownloadMV server Send message failed");
            return;
        }
        if(recv(clientfd,block,1,0) == -1){
            perror("TCPKernel::DownloadMV server recv Client ACK failed");
            return;
        }
        bzero(buf,DEF_BUFF_SIZE);
        bzero(block,2);
    }


    printf("TcpKernel::PlayMV send file finished\n");
    close(fd);

}

//MV播放请求
void TcpKernel::PlayMV(int clientfd,char* szbuf,int nlen)
{

}
