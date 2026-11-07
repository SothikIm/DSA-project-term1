#include <iostream>
#include <mariadb/mysql.h>   // or <mysql.h> if that's how you set it up
#include <cstdlib>
#include<map>
#include "load_env.hpp"
using namespace std;
int main() {
    MYSQL *con;
    MYSQL_RES *res;
    MYSQL_ROW row;
    auto env = load_env();
    const char *host = env["HOST"].c_str();
    const char *user = env["USER"].c_str();
    const char *pass = env["PASSWORD"].c_str();
    const char *db = env["DB"].c_str();
    int port = stoi(env["PORT"]);

    // connect to mysql connection
    con = mysql_init(nullptr);
    if(con == nullptr){
        cerr << "mysql_init() fail\n";
        return 1;
    }

    // connect to database
    if(mysql_real_connect(con, host, user, pass, db, port, nullptr, 0) == nullptr){
        cerr << "connection error : " << mysql_error(con) << endl;
        mysql_close(con);
        return 1;
    }

    cout << "Connect successful\n\n";

    // run query
    char* query = "SELECT * FROM student;";

    if(mysql_query(con, query)){
        cerr << "Query Error: " << mysql_error(con) << "\n";
        mysql_close(con);
        return 1;
    }

    res = mysql_store_result(con);
    if(res != nullptr){
        cout << "Query : \n";
        cout << "------------------------------------\n";
        while((row = mysql_fetch_row(res)) != nullptr){
            cout << (row[0] ? row[0] : "NULL") << "\t"
                << (row[1] ? row[1] : "NULL") << "\t"
                << (row[2] ? row[2] : "NULL") << "\t"
                << (row[3] ? row[3] : "NULL") << "\n"
                << (row[4] ? row[4] : "NULL") << "\n";
        }
        mysql_free_result(res);
    }
    mysql_close(con);
    return 0;

    return 0;
}
