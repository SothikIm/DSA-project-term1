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

    bool isConflictAccount(const int& id){
        for(auto itr = acc.begin(); itr != acc.end(); ++itr){
            if(itr->studentId == id){
                return true;
            }
        }
        return false;
    }
public:
    LoginAccount(Database& db): db(db) {
        string query = "SELECT s.student_id,"
                        "CONCAT(s.f_name, ' ', s.l_name) AS student_name, "
                        "s.email, "
                        "a.`password` "
                        "FROM `Account` a JOIN Student s ON a.student_id = s.student_id;";
        acc = db.runQuery<Account>(query);
    }
    bool login(const string& email, const string& password){
        for(auto itr = acc.begin(); itr != acc.end(); ++itr){
            if(itr->email == email && itr->password == password){
                return true;
            }
        }
        return false;
    }

    void displayAllAccount(){
        acc.display();
    }
};

#endif
