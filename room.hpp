#ifndef INCLUDE_ROOM
#define INCLUDE_ROOM
#include<iostream>
#include"linked_list.hpp"
#include"struct.hpp"
#include"database.hpp"
class RoomList{
private:
    Database& db;
    LinkList<Room> r;
public:
    RoomList(Database& db): db(db){
        string sql = "SELECT room_code, capacity FROM Room;";
        r = db.runQuery<Room>(sql);
    }
    void displayAllRoom(){
        r.display();
    }
};



#endif // NOT_INCLUDE_ROOM
