#ifndef _PACKDEF_H
#define _PACKDEF_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "err_str.h"
#include <malloc.h>
#include <fcntl.h>

#include<iostream>
#include<map>
#include<list>


//边界值
#define _DEF_SIZE 45
#define _DEF_BUFFERSIZE 1000
#define _DEF_PORT 8000
#define _DEF_SERVERIP "192.168.115.128"  //TODO


#define TRUE true
#define FALSE false


#define _DEF_LISTEN 128
#define _DEF_EPOLLSIZE 4096
#define _DEF_IPSIZE 16
#define _DEF_COUNT 10
#define _DEF_TIMEOUT 10
#define _DEF_SQLIEN 400



#define BOOL bool
#define DEF_PACK_BASE  (10000)


//注册
#define  DEF_PACK_REGISTER_RQ    (DEF_PACK_BASE + 0)
#define  DEF_PACK_REGISTER_RS    (DEF_PACK_BASE + 1)
//登录
#define  DEF_PACK_LOGIN_RQ    (DEF_PACK_BASE + 2)
#define  DEF_PACK_LOGIN_RS    (DEF_PACK_BASE + 3)
//播放MV请求
#define  DEF_PACK_PLAY_MV_RQ      (DEF_PACK_BASE + 4)
#define  DEF_PACK_PLAY_MV_RS      (DEF_PACK_BASE + 5)
//下载MV请求
#define  DEF_PACK_DOWNLOAD_MV_RQ      (DEF_PACK_BASE + 6)
#define  DEF_PACK_DOWNLOAD_MV_RS      (DEF_PACK_BASE + 7)
/*//创建房间
#define DEF_PACK_CREATEROOM_RQ  (DEF_PACK_BASE + 4)
#define DEF_PACK_CREATEROOM_RS  (DEF_PACK_BASE + 5)
//加入房间
#define DEF_PACK_JOINROOM_RQ  (DEF_PACK_BASE + 6)
#define DEF_PACK_JOINROOM_RS  (DEF_PACK_BASE + 7)
//房间列表请求
#define DEF_PACK_ROOM_MEMBER    (DEF_PACK_BASE + 8)
//音频数据
#define DEF_PACK_AUDIO_FRAME    (DEF_PACK_BASE + 9)
//退出房间请求
#define DEF_PACK_LEAVEROOM_RQ   (DEF_PACK_BASE + 10)
//退出房间回复
#define DEF_PACK_LEAVEROOM_RS   (DEF_PACK_BASE + 11)
*/

//注册请求结果
#define userid_is_exist      0
#define register_sucess      1

//登录请求结果
#define userid_no_exist      0
#define password_error       1
#define login_sucess         2
#define user_online          3

//创建房间结果
#define room_is_exist        0
#define create_success       1

//加入房间结果
#define room_no_exist        0
#define join_success         1

//上传请求结果
#define file_is_exist        0
#define file_uploaded        1
#define file_uploadrq_sucess 2
#define file_upload_refuse   3

//上传回复结果
#define fileblock_failed     0
#define fileblock_success    1

//下载请求结果
#define file_downrq_failed   0
#define file_downrq_success  1

#define _downloadfileblock_fail  0
#define _downloadfileblock_success	1

#define DEF_PACK_COUNT (100)

#define MAX_PATH            (280 )
#define MAX_SIZE            (60  )
#define DEF_HOBBY_COUNT     (8  )
#define MAX_CONTENT_LEN     (4096 )


/////////////////////网络//////////////////////////////////////


#define DEF_MAX_BUF	  1024
#define DEF_BUFF_SIZE	  4096


typedef int PackType;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//登录请求
typedef struct STRU_LOGIN_RQ
{
    STRU_LOGIN_RQ()
    {
        m_nType = DEF_PACK_LOGIN_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户ID
    char     m_szPassword[MAX_SIZE];  //密码
} STRU_LOGIN_RQ;


//登录回复
typedef struct STRU_LOGIN_RS
{
    STRU_LOGIN_RS()
    {
        m_nType= DEF_PACK_LOGIN_RS;
        m_UserID = 0;
        m_lResult = 0;
    }
    PackType m_nType;   //包类型
    int  m_UserID;
    int  m_lResult ; //注册结果

} STRU_LOGIN_RS;


//注册请求
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ()
    {
        m_nType = DEF_PACK_REGISTER_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户名
    char     m_szPassword[MAX_SIZE];  //密码

} STRU_REGISTER_RQ;

//注册回复
typedef struct STRU_REGISTER_RS
{
    STRU_REGISTER_RS()
    {
        m_nType= DEF_PACK_REGISTER_RS;
        m_lResult = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ; //注册结果

} STRU_REGISTER_RS;
//MV请求
typedef struct STRU_PLAY_MV_RQ
{
    STRU_PLAY_MV_RQ(){//构造函数
        m_nType = DEF_PACK_PLAY_MV_RQ;
        bzero(m_szMVName,MAX_SIZE);
    }
    PackType m_nType; //包类型
    char m_szMVName[MAX_SIZE];//请求的MV的名字
}STRU_PLAY_MV_RQ;
//MV回复
typedef struct STRU_PLAY_MV_RS
{
    STRU_PLAY_MV_RS(){//构造函数
        m_nType = DEF_PACK_PLAY_MV_RS;
        bzero(m_szMVName,MAX_SIZE);
        bzero(m_szMVContent,DEF_BUFF_SIZE);
        m_lResult = 0;
    }
    PackType m_nType; //包类型
    char m_szMVContent[DEF_BUFF_SIZE];//请求的MV的数据
    char m_szMVName[MAX_SIZE];//请求的MV的名字
    long long m_nPackSize;//数据包大小
    int m_lResult;   //请求MV结果
} STRU_PLAY_MV_RS;

//下载MV请求
typedef struct STRU_DOWNLOAD_MV_RQ
{
    STRU_DOWNLOAD_MV_RQ(){//构造函数
        m_nType = DEF_PACK_DOWNLOAD_MV_RQ;
        memset(m_szMVName,0,MAX_SIZE);
    }
    PackType m_nType; //包类型
    char m_szMVName[MAX_SIZE];//请求的MV的名字
} STRU_DOWNLOAD_MV_RQ;
//下载MV回复
typedef struct STRU_DOWNLOAD_MV_RS
{
    STRU_DOWNLOAD_MV_RS(){//构造函数
        m_nType = DEF_PACK_DOWNLOAD_MV_RS;
        memset(m_szMVName,0,MAX_SIZE);
        m_lResult = 0;
        m_nThreadNum = 0;
        m_nPackSize = 0;
    }
    PackType m_nType; //包类型
    char m_szMVName[MAX_SIZE];//请求的MV的名字
    int m_nThreadNum;//需要创建的线程数
    int m_lResult;   //请求MV结果
    long long m_nPackSize;//数据包大小
} STRU_DOWNLOAD_MV_RS;
#endif



