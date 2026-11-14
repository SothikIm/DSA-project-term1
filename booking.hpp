#ifndef BOOKING_HPP_INCLUDED
#define BOOKING_HPP_INCLUDED

#include<iostream>
#include<cstdlib>
#include<string>
#include "linked_list.hpp"
#include "struct.hpp"
#include "database.hpp"

class Booking{
private:
    Database& db;
    LinkList<Reservation> existBooking;

    bool isConflictTime(DateTime startA, DateTime endA, DateTime startB, DateTime endB){
        return (startA < endB) && (endA > startB);
    }

    bool hasConflictRoom(Reservation newBooking){
        if(newBooking.startTime <= newBooking.endTime){
            throw runtime_error("Invalid reservation: end time must be after start time.\n");
        }

        if(newBooking.isWorkingHour())
            throw runtime_error("Invalid reservation: event must hold in working hour.\n");

        for(const Reservation& booking: existBooking){
            if(booking.roomCode != newBooking.roomCode)
                continue;
            if(!booking.startTime.isSameDay(newBooking.startTime))
                continue;
            if(isConflictTime(booking.startTime, booking.endTime, newBooking.startTime, newBooking.endTime)){
                return true;
            }
        }
        return false;
    }
public:
    Booking(Database& db): db(db){
        existBooking = db.runQuery<Reservation>("SELECT * FROM reservation;");
    }

    void addBooking(const Reservation& newBooking){
        if(hasConflictRoom(newBooking))
            throw runtime_error("Conflict detected! Room already book.\n");
        string sql = "INSERT INTO Reservation (start_time, end_time, purpose, create_at, room_code, student_id) VALUES (" +
                        newBooking.startTime.toMysqlString() + "', '" +
                        newBooking.endTime.toMysqlString() + "', '" +
                        newBooking.purpose + "', '" +
                        newBooking.createAt.toMysqlString() + "', " +
                        newBooking.roomCode + ", " +
                        to_string(newBooking.studentId) + ");";
        db.execute(sql);
        existBooking.pushBack(newBooking);
    }

    void display(){
        existBooking.display();
    }
};



















#endif
