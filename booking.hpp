#ifndef BOOKING_HPP_INCLUDED
#define BOOKING_HPP_INCLUDED

#include<iostream>
#include<cstdlib>
#include<string>
#include<cctype>
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
        if(newBooking.startTime >= newBooking.endTime){
            throw runtime_error("Invalid reservation: end time must be after start time.\n");
        }

        if(!newBooking.isWorkingHour())
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
        string sql = R"(
            WITH RankedProcedure AS (
                SELECT
                    p.*,
                    ROW_NUMBER() OVER (
                        PARTITION BY p.reservation_id
                        ORDER BY p.procedure_time DESC
                    ) AS rn
                FROM `Procedure` p
            )
            SELECT
                r.*,
                ps.procedure_status AS last_procedure
            FROM Reservation r
            LEFT JOIN RankedProcedure rp
                ON r.reservation_id = rp.reservation_id
                AND rp.rn = 1
            LEFT JOIN Procedure_status ps
                ON rp.procedure_status_id = ps.procedure_status_id;
        )";
        existBooking = db.runQuery<Reservation>(sql);
    }

    void addBooking(Reservation& newBooking){
        if(hasConflictRoom(newBooking))
            throw runtime_error("Conflict detected! Room already book.\n");
        string sql = "INSERT INTO Reservation (start_time, end_time, purpose, room_code, student_id) VALUES ('" +
                        newBooking.startTime.toMysqlString() + "', '" +
                        newBooking.endTime.toMysqlString() + "', '" +
                        newBooking.purpose + "', '" +
                        newBooking.roomCode + "', " +
                        to_string(newBooking.studentId) + ");";
        newBooking.id = db.insert(sql);
        sql = "INSERT INTO `Procedure` (procedure_time, reservation_id, procedure_status_id) VALUES"
                "(NOW(), LAST_INSERT_ID(), 1);";
        db.insert(sql);
        newBooking.procedure = "Created";
        existBooking.pushBack(newBooking);
    }

    void cancelBooking(int& bookingId){
        for(auto itr = existBooking.begin(); itr != existBooking.end(); ++itr){
            if(bookingId == itr->id){
                cout << *itr;
                char ans;
                cout << "Are your sure you want to booking this?(Y or N): ";
                cin >> ans;
                if(tolower(ans) == 'y'){
                    string sql = "DELETE FROM Reservation WHERE reservation_id = " + to_string(bookingId) + ";";
                    db.execute(sql);
                    existBooking.pop(*itr);
                    cout << "Cancel successfully.\n";
                }
                return;
            }
        }
        throw runtime_error("Reservation not found.\n");
    }

    void display(){
        existBooking.display();
    }
};



















#endif
