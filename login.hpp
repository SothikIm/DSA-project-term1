#ifndef LOGIN_INCLUDED
#define LOGIN_INCLUDED

#include<iostream>
#include<string>
#include "database.hpp"
#include "linked_list.hpp"
#include "struct.hpp"
using namespace std;

class LoginAccount{
private:
    Database& db;
    LinkList<Account> acc;
public:
    LoginAccount(Database& db): db(db) {
        string query = "SELECT "
                        "CONCAT(s.f_name, ' ', s.l_name) AS student_name, "
                        "s.email, "
                        "a.`password` "
                        "FROM `Account` a JOIN Student s ON a.student_id = s.student_id;";
        acc = db.runQuery<Account>(query);
    }
//    bool login(const string& name, const string& email){
//
//    }

    void displayAllAccount(){
        acc.display();
    }
};

#endif
