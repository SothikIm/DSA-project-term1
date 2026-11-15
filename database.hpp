#ifndef DATABASE_INCLUDED
#define DATABASE_INCLUDED


#include <iostream>
#include <mariadb/mysql.h>
#include <cstdlib>
#include<map>
#include<vector>
#include "load_env.hpp"
#include "linked_list.hpp"
#include "struct.hpp"
using namespace std;



class Database{
private:
    MYSQL* con;
public:
    Database(const char* host, const char* user, const char* password, const char* db, int port){
        con = mysql_init(nullptr);
        if(!con){
            throw runtime_error("mysql_init failed()");
        }
        if(!mysql_real_connect(con, host, user, password, db, port, nullptr, 0)){
            mysql_close(con);
            throw runtime_error(mysql_error(con));
        }
    }

    ~Database(){
        if(con){
            mysql_close(con);
        }
    }

    void execute(const string& query){
        if(mysql_query(con, query.c_str()))
            throw runtime_error(mysql_error(con));
    }

    my_ulonglong insert(const string& query){
        if(mysql_query(con, query.c_str()))
            throw runtime_error(mysql_error(con));
        return mysql_insert_id(con);
    }

    template <typename T>
    LinkList<T> runQuery(const string& query){
        if(mysql_query(con, query.c_str()))
            throw runtime_error(mysql_error(con));
        MYSQL_RES* res = mysql_store_result(con);
        if(!res){
            if(mysql_field_count(con) == 0){
                return LinkList<T>();
            }
            throw runtime_error(mysql_error(con));
        }
        LinkList<T> result;
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res))){
            result.pushBack(T(row));
        }
        mysql_free_result(res);
        return result;
    }
};



#endif // DATABASE_INCLUDED
