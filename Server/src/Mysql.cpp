#include "Mysql.h"



int CMysql::ConnectMysql(string server,string user,string password,string database)
{
    conn = NULL;
    conn = mysql_init(NULL);
    mysql_set_character_set(conn,"utf8");
    if(!mysql_real_connect(conn,server.c_str(),user.c_str(),password.c_str(),database.c_str(),0,NULL,0))
    {
        return FALSE;
    }
    return TRUE;
}

int CMysql::SelectMysql(char* szSql,int nColumn,list<string>& lst)
{
    results = NULL;
    if(mysql_query(conn,szSql))return FALSE;
    results = mysql_store_result(conn);
    if(NULL == results)return FALSE;
    while((record = mysql_fetch_row(results)))
    {

        for(int i=0; i<nColumn; i++)
        {
            lst.push_back( record[i] );
       //     q_Push(pQueue,(void*)record[i]);
        }
    }
    return TRUE;
}


int CMysql::UpdataMysql(char* szsql)
{
    if(!szsql)return FALSE;
    if(mysql_query(conn,szsql))return FALSE;

    return TRUE;
}

void CMysql::DisConnect()
{
    mysql_close(conn);
}
