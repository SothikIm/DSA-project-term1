#ifndef STRUCT_HPP_INCLUDED
#define STRUCT_HPP_INCLUDED
#include <iostream>
#include <string>
#include <iomanip>
#include <tuple>
#include <mariadb/mysql.h>
#include <cstdlib>
#include <sstream>
#include <ctime>
using namespace std;

enum ProcedureStatus {
    Created  = 1, CheckIn, CheckOut, Cancel, Delay
};

struct Account{
    int studentId;
    string name;
    string email;
    string password;

    Account(int id, string name, string email, string password):
        studentId(id), name(name), email(email), password(password) {}
    Account() {}

    Account(MYSQL_ROW row){
        studentId = row[0] ? stoi(row[0]) : 0;
        name = row[1] ? string(row[1]) : "";
        email = row[2] ? string(row[2]) : "";
        password = row[3] ? string(row[3]) : "";
    }
    friend ostream& operator<<(ostream& os, const Account& acc) {
        os << "{" << acc.studentId << " " << acc.name << ", " << acc.email << "}";
        return os;
    }
};

struct Building{
    int id;
    string name;

    Building() {}
    Building(int id, string name): id(id), name(name) {}
    Building(MYSQL_ROW row){
        id = (row[0] ? stoi(row[0]) : 0);
        name = (row[1] ? row[1] : "");
    }
    friend ostream& operator<<(ostream& os, const Building& b) {
        os << "{" << b.id << ", " << b.name << "}";
        return os;
    }
};

struct DateTime {
    int year;
    int month;   // 1 - 12
    int day;     // 1 - 31
    int hour;    // 0 - 23
    int minute;  // 0 - 59
    int second;

    // Check if two DateTimes fall on the exact same calendar day
    bool isSameDay(const DateTime& other) const {
        return year == other.year && month == other.month && day == other.day;
    }

    DateTime() {}

    DateTime(const char* str){
        char dash1, dash2, colon1, colon2;
        stringstream ss(str);
        ss >> year >> dash1 >> month >> dash2 >> day
           >> hour >> colon1 >> minute >> colon2 >> second;
    }

    DateTime (int year, int month, int day, int hour, int min, int sec) {
        this->year = year;
        this->month = month;
        this->day = day;
        this->hour = hour;
        this->minute = min;
        this->second = sec;
    }

    DateTime operator=(const char* str){
        if (str && str[0] != '\0') {
            char dash1, dash2, colon1, colon2;
            stringstream ss(str);
            ss >> year >> dash1 >> month >> dash2 >> day
               >> hour >> colon1 >> minute >> colon2 >> second;
        }
        return *this;
    }


    // Convert to a tuple for clean lexicographical ordering
    auto toTuple() const {
        return tie(year, month, day, hour, minute, second);
    }

    bool operator<(const DateTime& other) const {
        return toTuple() < other.toTuple();
    }

    bool operator<=(const DateTime& other) const {
        return toTuple() <= other.toTuple();
    }

    bool operator==(const DateTime& other) const {
        return toTuple() == other.toTuple();
    }

    bool operator>(const DateTime& other) const {
        return toTuple() > other.toTuple();
    }

    bool operator>=(const DateTime& other) const {
        return toTuple() >= other.toTuple();
    }

    void printDateTime() {
        cout << year << "-"
              << setfill('0') << setw(2) << month << "-"
              << setfill('0') << setw(2) << day << " "
              << setfill('0') << setw(2) << hour << ":"
              << setfill('0') << setw(2) << minute << ":"
              << setfill('0') << setw(2) << second << endl;
    }

    friend ostream& operator<<(ostream& os, const DateTime& dt){
        os << dt.year << "-"
              << setfill('0') << setw(2) << dt.month << "-"
              << setfill('0') << setw(2) << dt.day << " "
              << setfill('0') << setw(2) << dt.hour << ":"
              << setfill('0') << setw(2) << dt.minute << ":"
              << setfill('0') << setw(2) << dt.second << endl;
        return os;
    }

    friend istream& operator>>(istream& is, DateTime& dt){
        cout << "Enter Year Month Day Hour Minute Second: ";
        is >> dt.year >> dt.month >> dt.day >> dt.hour >> dt.minute >> dt.second;
        return is;
    }

    string toMysqlString() const {
        ostringstream oss;
        oss << setfill('0')
            << setw(4) << year << "-"
            << setw(2) << month << "-"
            << setw(2) << day << " "
            << setw(2) << hour << ":"
            << setw(2) << minute << ":"
            << setw(2) << second;
        return oss.str();
    }

    static DateTime now() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    return DateTime(
        lt->tm_year + 1900,
        lt->tm_mon + 1,
        lt->tm_mday,
        lt->tm_hour,
        lt->tm_min,
        lt->tm_sec
    );
}
};

struct Reservation {
    int id;
    DateTime startTime;
    DateTime endTime;
    string purpose;
    DateTime createAt;
    string roomCode;
    int studentId;
    string procedure;

    Reservation() {}
    Reservation(MYSQL_ROW row){
        id = (row[0] ? stoi(row[0]) : 0);
        startTime = row[1] ? row[1] : "";
        endTime = row[2] ? row[2] : "";
        purpose = row[3] ? row[3] : "";
        createAt = row[4] ? row[4] : "";
        roomCode = row[5] ? row[5] : "";
        studentId = row[6] ? stoi(row[6]) : 0;
        procedure = row[7] ? row[7] : "Created";
    }

    bool isWorkingHour(){
        bool afterStart = (startTime.hour >= 8);
        bool beforeEnd  = (endTime.hour < 17) || (endTime.hour == 17 && endTime.minute == 0);
        return afterStart && beforeEnd;
    }

    friend ostream& operator<<(ostream& os, const Reservation& res){
        os << "{ " << res.id << ", " << res.startTime << ", " << res.endTime
           << ", " << res.purpose << ", " << res.createAt << ", " << res.roomCode
           << ", " << res.studentId << ", " << res.procedure << "}\n";
        return os;
    }

    friend istream& operator>>(istream& is, Reservation& res){
        cout << "Enter start time: ";
        is >> res.startTime;
        cout << "Enter end time: ";
        is >> res.endTime;
        cout << "Enter purpose: ";
        getline(is >> ws, res.purpose);
        cout << "Enter room code or name: ";
        is >> res.roomCode;
        cout << "Enter your id: ";
        is >> res.studentId;
        return is;
    }

    bool operator==(const Reservation& other){
        return this->id == other.id;
    }
};

struct Room{
    string roomCode;
    int capacity;

    Room() {}
    Room(MYSQL_ROW row){
        roomCode = row[0] ? row[0] : "Unknown";
        capacity = row[1] ? stoi(row[1]) : 0;
    }

    friend ostream& operator<<(ostream& os, const Room& r){
        os << "{ " << r.roomCode << ", " << r.capacity << "}\n";
        return os;
    }
};


#endif // STRUCT_HPP_INCLUDED
