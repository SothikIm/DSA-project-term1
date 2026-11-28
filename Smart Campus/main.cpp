#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <ctime>
#include <cctype>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <mariadb/mysql.h>

using namespace std;

// ==========================================
// 1. LOAD_ENV
// ==========================================
string trim(string str) {
    size_t first = str.find_first_not_of(" \t\r\n\"'");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n\"'");
    return str.substr(first, (last - first + 1));
}

map<string, string> load_env(const string& filepath = ".env") {
    map<string, string> env;
    ifstream ifs(filepath);
    if (!ifs.is_open()) {
        cerr << "Can't open file: " << filepath << "\n";
        return env;
    }
    string line;
    while (getline(ifs >> ws, line)) {
        string trimmed_line = trim(line);
        if (trimmed_line.empty() || trimmed_line[0] == '#') continue;
        size_t eq_pos = line.find('=');
        if (eq_pos != string::npos) {
            string key = trim(line.substr(0, eq_pos));
            string value = trim(line.substr(eq_pos + 1));
            env[key] = value;
        }
    }
    return env;
}

// ==========================================
// 2. LINKED LIST
// ==========================================
template <typename T>
class LinkList {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

    int size;
    Node* head;
    Node* tail;

public:
    LinkList(const T& d) : size(1), head(new Node(d)), tail(head) {}

    LinkList() : size(0), head(nullptr), tail(nullptr) {}

    LinkList(const T* arr, const int arrSize) : size(arrSize) {
        if (arrSize <= 0 || !arr) {
            head = nullptr;
            tail = nullptr;
            size = 0;
            return;
        }

        head = new Node(arr[0]);
        tail = head;
        for (int i = 1; i < arrSize; i++) {
            Node* t = new Node(arr[i]);
            tail->next = t;
            tail = t;
        }
    }

    LinkList(const LinkList& other) : size(0), head(nullptr), tail(nullptr) {
        Node* cur = other.head;
        while (cur) {
            pushBack(cur->data);
            cur = cur->next;
        }
    }

    LinkList(LinkList&& other) noexcept : size(other.size), head(other.head), tail(other.tail) {
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }

    LinkList& operator=(const LinkList& other) {
        if (this != &other) {
            clear();
            Node* cur = other.head;
            while (cur) {
                pushBack(cur->data);
                cur = cur->next;
            }
        }
        return *this;
    }

    LinkList& operator=(LinkList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            size = other.size;

            other.head = nullptr;
            other.tail = nullptr;
            other.size = 0;
        }
        return *this;
    }

    int getSize() const {
        return size;
    }

    void display() const {
        if (isEmpty()) {
            cout << "The list is empty" << endl;
            return;
        }
        cout << "[";
        for (Node* p = head; p != nullptr; p = p->next) {
            if (p != head) {
                cout << ", " << endl;
            }
            cout << p->data;
        }
        cout << "]" << endl;
    }

    void pushBack(const T& val) {
        Node* t = new Node(val);
        if (!head) {
            head = t;
            tail = head;
        } else {
            tail->next = t;
            tail = t;
        }
        size++;
    }

    void pushFront(const T& val) {
        Node* t = new Node(val);
        t->next = head;
        head = t;
        if (!tail)
            tail = t;
        size++;
    }

    void insert(const T& val, const int& index) {
        try {
            if (index > size || index < 0)
                throw invalid_argument("IndexError");
            else if (index == 0) {
                pushFront(val);
                return;
            } else if (index == size) {
                pushBack(val);
                return;
            }
            Node* p = head;
            for (int pos = 0; pos < index - 1; pos++) {
                p = p->next;
            }
            Node* t = new Node(val);
            t->next = p->next;
            p->next = t;
            size++;
        } catch (invalid_argument& e) {
            cerr << e.what() << endl;
        }
    }

    bool find(const T& key) const {
        for (Node* i = head; i != nullptr; i = i->next) {
            if (i->data == key)
                return true;
        }
        return false;
    }

    bool isEmpty() const {
        return !head;
    }

    void sort() {
        if (!head || !head->next)
            return;
        bool swapped;

        do {
            swapped = false;
            Node* cur = head;
            Node* prev = nullptr;
            while (cur->next) {
                Node* next = cur->next;
                if (cur->data > next->data) {
                    cur->next = next->next;
                    next->next = cur;
                    if (prev == nullptr)
                        head = next;
                    else
                        prev->next = next;
                    prev = next;
                    swapped = true;
                } else {
                    prev = cur;
                    cur = cur->next;
                }
            }
        } while (swapped);
    }

    void pop(const T& key) {
        if (!head) {
            cout << "The list is empty" << endl;
            return;
        } else if (key == head->data) {
            Node* p = head->next;
            delete head;
            head = p;
            if (!head)
                tail = nullptr;
            cout << "Delete successfully" << endl;
            size--;
        } else {
            bool found = false;
            Node* cur = head->next;
            Node* prev = head;
            while (cur != nullptr) {
                if (key == cur->data) {
                    if (cur == tail)
                        tail = prev;
                    prev->next = cur->next;
                    delete cur;
                    found = true;
                    size--;
                    break;
                } else {
                    prev = cur;
                    cur = cur->next;
                }
            }
            if (!found)
                cout << "Element not found" << endl;
        }
    }

    void pop_at(const int& index) {
        if (index >= size || index < 0)
            return;
        Node* cur = head;
        Node* prev = nullptr;
        for (int i = 0; i < index; i++) {
            prev = cur;
            cur = cur->next;
        }

        if (prev) {
            prev->next = cur->next;
        } else {
            head = cur->next;
        }

        if (cur == tail)
            tail = prev;
        delete cur;
        size--;
    }

    T& at(const int& index) {
        if (index >= size || index < 0) {
            throw out_of_range("Index out of range");
        }
        Node* cur = head;
        for (int i = 0; i < index; i++) {
            cur = cur->next;
        }
        return cur->data;
    }

    void clear() {
        Node* cur = head;
        while (cur) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* node = nullptr) : current(node) {}

        T& operator*() const {
            return current->data;
        }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        T* operator->() const {
            return &(current->data);
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }
    };

    Iterator begin() {
        return Iterator(head);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    Iterator begin() const {
        return Iterator(head);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    ~LinkList() {
        clear();
    }

    void update(const T& old_data, const T& new_data) {
        Node* cur = head;
        while (cur != nullptr) {
            if (cur->data == old_data) {
                cur->data = new_data;
                return;
            }
            cur = cur->next;
        }
        cout << "The old data does not exist" << endl;
    }
};

// ==========================================
// 3. DOMAIN STRUCTS
// ==========================================
enum ProcedureStatus {
    Created = 1, CheckIn, CheckOut, Cancel, Delay
};

struct Account {
    int studentId = 0;
    string name;
    string email;
    string password;

    Account() = default;
    Account(int id, string name, string email, string password)
        : studentId(id), name(std::move(name)), email(std::move(email)), password(std::move(password)) {}

    Account(MYSQL_ROW row) {
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

struct Building {
    int id = 0;
    string name;

    Building() = default;
    Building(int id, string name) : id(id), name(std::move(name)) {}

    Building(MYSQL_ROW row) {
        id = (row[0] ? stoi(row[0]) : 0);
        name = (row[1] ? row[1] : "");
    }

    friend ostream& operator<<(ostream& os, const Building& b) {
        os << "{" << b.id << ", " << b.name << "}";
        return os;
    }
};

struct DateTime {
    int year = 2026;
    int month = 1;   // 1 - 12
    int day = 1;     // 1 - 31
    int hour = 0;    // 0 - 23
    int minute = 0;  // 0 - 59
    int second = 0;

    bool isSameDay(const DateTime& other) const {
        return year == other.year && month == other.month && day == other.day;
    }

    DateTime() = default;

    DateTime(const char* str) {
        if (str && str[0] != '\0') {
            char dash1, dash2, colon1, colon2;
            stringstream ss(str);
            ss >> year >> dash1 >> month >> dash2 >> day
               >> hour >> colon1 >> minute >> colon2 >> second;
        }
    }

    DateTime(int y, int m, int d, int h, int min, int sec)
        : year(y), month(m), day(d), hour(h), minute(min), second(sec) {}

    DateTime& operator=(const char* str) {
        if (str && str[0] != '\0') {
            char dash1, dash2, colon1, colon2;
            stringstream ss(str);
            ss >> year >> dash1 >> month >> dash2 >> day
               >> hour >> colon1 >> minute >> colon2 >> second;
        }
        return *this;
    }

    auto toTuple() const {
        return tie(year, month, day, hour, minute, second);
    }

    bool operator<(const DateTime& other) const { return toTuple() < other.toTuple(); }
    bool operator<=(const DateTime& other) const { return toTuple() <= other.toTuple(); }
    bool operator==(const DateTime& other) const { return toTuple() == other.toTuple(); }
    bool operator>(const DateTime& other) const { return toTuple() > other.toTuple(); }
    bool operator>=(const DateTime& other) const { return toTuple() >= other.toTuple(); }

    void printDateTime() const {
        cout << year << "-"
             << setfill('0') << setw(2) << month << "-"
             << setfill('0') << setw(2) << day << " "
             << setfill('0') << setw(2) << hour << ":"
             << setfill('0') << setw(2) << minute << ":"
             << setfill('0') << setw(2) << second << endl;
    }

    friend ostream& operator<<(ostream& os, const DateTime& dt) {
        os << dt.year << "-"
           << setfill('0') << setw(2) << dt.month << "-"
           << setfill('0') << setw(2) << dt.day << " "
           << setfill('0') << setw(2) << dt.hour << ":"
           << setfill('0') << setw(2) << dt.minute << ":"
           << setfill('0') << setw(2) << dt.second;
        return os;
    }

    friend istream& operator>>(istream& is, DateTime& dt) {
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
    int id = 0;
    DateTime startTime;
    DateTime endTime;
    string purpose;
    DateTime createAt;
    string roomCode;
    int studentId = 0;
    string procedure;

    Reservation() = default;

    Reservation(MYSQL_ROW row) {
        id = (row[0] ? stoi(row[0]) : 0);
        startTime = row[1] ? row[1] : "";
        endTime = row[2] ? row[2] : "";
        purpose = row[3] ? row[3] : "";
        createAt = row[4] ? row[4] : "";
        roomCode = row[5] ? row[5] : "";
        studentId = row[6] ? stoi(row[6]) : 0;
        procedure = row[7] ? row[7] : "Created";
    }

    bool isWorkingHour() const {
        bool afterStart = (startTime.hour >= 8);
        bool beforeEnd = (endTime.hour < 17) || (endTime.hour == 17 && endTime.minute == 0);
        return afterStart && beforeEnd;
    }

    friend ostream& operator<<(ostream& os, const Reservation& res) {
        os << "{ " << res.id << ", " << res.startTime << ", " << res.endTime
           << ", " << res.purpose << ", " << res.createAt << ", " << res.roomCode
           << ", " << res.studentId << ", " << res.procedure << "}\n";
        return os;
    }

    friend istream& operator>>(istream& is, Reservation& res) {
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

    bool operator==(const Reservation& other) const {
        return this->id == other.id;
    }
};

struct Room {
    string roomCode;
    int capacity = 0;

    Room() = default;

    Room(MYSQL_ROW row) {
        roomCode = row[0] ? row[0] : "Unknown";
        capacity = row[1] ? stoi(row[1]) : 0;
    }

    friend ostream& operator<<(ostream& os, const Room& r) {
        os << "{ " << r.roomCode << ", " << r.capacity << "}\n";
        return os;
    }
};

// ==========================================
// 4. DATABASE
// ==========================================
class Database {
private:
    MYSQL* con = nullptr;

public:
    Database(const char* host, const char* user, const char* password, const char* db, int port) {
        con = mysql_init(nullptr);
        if (!con) {
            throw runtime_error("mysql_init failed()");
        }
        if (!mysql_real_connect(con, host, user, password, db, port, nullptr, 0)) {
            string errMsg = mysql_error(con);
            mysql_close(con);
            con = nullptr;
            throw runtime_error(errMsg);
        }
    }

    ~Database() {
        if (con) {
            mysql_close(con);
            con = nullptr;
        }
    }

    void execute(const string& query) {
        if (mysql_query(con, query.c_str()))
            throw runtime_error(mysql_error(con));
    }

    my_ulonglong insert(const string& query) {
        if (mysql_query(con, query.c_str()))
            throw runtime_error(mysql_error(con));
        return mysql_insert_id(con);
    }

    template <typename T>
    LinkList<T> runQuery(const string& query) {
        if (mysql_query(con, query.c_str()))
            throw runtime_error(mysql_error(con));
        MYSQL_RES* res = mysql_store_result(con);
        if (!res) {
            if (mysql_field_count(con) == 0) {
                return LinkList<T>();
            }
            throw runtime_error(mysql_error(con));
        }
        LinkList<T> result;
        MYSQL_ROW row;
        int size = mysql_num_fields(res);
        while ((row = mysql_fetch_row(res))) {
            if constexpr (is_same_v<T, string>) {
                for (int i = 0; i < size; i++) {
                    result.pushBack(row[i] ? string(row[i]) : "");
                }
            } else {
                result.pushBack(T(row));
            }
        }
        mysql_free_result(res);
        return result;
    }
};

// ==========================================
// 5. LOGIN SERVICE
// ==========================================
class LoginAccount {
private:
    Database& db;
    LinkList<Account> acc;

public:
    LoginAccount(Database& db) : db(db) {
        string query = "SELECT s.student_id,"
                       "CONCAT(s.f_name, ' ', s.l_name) AS student_name, "
                       "s.email, "
                       "a.`password` "
                       "FROM `Account` a JOIN Student s ON a.student_id = s.student_id;";
        acc = db.runQuery<Account>(query);
    }

    bool login(const string& email, const string& password) {
        for (auto itr = acc.begin(); itr != acc.end(); ++itr) {
            if (itr->email == email && itr->password == password) {
                return true;
            }
        }
        return false;
    }

    void displayAllAccount() const {
        acc.display();
    }
};

// ==========================================
// 6. ROOM SERVICE
// ==========================================
class RoomList {
private:
    Database& db;
    LinkList<Room> r;

public:
    RoomList(Database& db) : db(db) {
        string sql = "SELECT room_code, capacity FROM Room;";
        r = db.runQuery<Room>(sql);
    }

    LinkList<Room>& getRooms() {
        return r;
    }

    const LinkList<Room>& getRooms() const {
        return r;
    }

    void displayAllRoom() const {
        r.display();
    }
};

// ==========================================
// 7. BOOKING SERVICE
// ==========================================
class Booking {
private:
    Database& db;
    LinkList<Reservation> existBooking;

    bool isConflictTime(DateTime startA, DateTime endA, DateTime startB, DateTime endB) const {
        return (startA < endB) && (endA > startB);
    }

    bool hasConflictRoom(const Reservation& newBooking) const {
        if (newBooking.startTime >= newBooking.endTime) {
            throw runtime_error("Invalid reservation: end time must be after start time.\n");
        }

        if (!newBooking.isWorkingHour())
            throw runtime_error("Invalid reservation: event must hold in working hour.\n");

        for (const Reservation& booking : existBooking) {
            if (booking.roomCode != newBooking.roomCode)
                continue;
            if (!booking.startTime.isSameDay(newBooking.startTime))
                continue;
            if (isConflictTime(booking.startTime, booking.endTime, newBooking.startTime, newBooking.endTime)) {
                return true;
            }
        }
        return false;
    }

public:
    Booking(Database& db) : db(db) {
        string sql = R"(
            WITH RankedProcedure AS (
                SELECT
                    p.*,
                    ROW_NUMBER() OVER (
                        PARTITION BY p.reservation_id
                        ORDER BY p.procedure_time DESC, p.procedure_status_id DESC
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

    void addBooking(Reservation& newBooking) {
        if (hasConflictRoom(newBooking))
            throw runtime_error("Conflict detected! Room already booked.\n");
        string sql = "INSERT INTO Reservation (start_time, end_time, purpose, room_code, student_id) VALUES ('" +
                     newBooking.startTime.toMysqlString() + "', '" +
                     newBooking.endTime.toMysqlString() + "', '" +
                     newBooking.purpose + "', '" +
                     newBooking.roomCode + "', " +
                     to_string(newBooking.studentId) + ");";
        newBooking.id = static_cast<int>(db.insert(sql));
        newBooking.createAt = DateTime::now();
        sql = "INSERT INTO `Procedure` (procedure_time, reservation_id, procedure_status_id) VALUES"
              "(NOW(), LAST_INSERT_ID(), 1);";
        db.insert(sql);
        newBooking.procedure = "Created";
        existBooking.pushBack(newBooking);
    }

    void updateBookingStatus(int bookingId, ProcedureStatus newStatus) {
        int statusId = static_cast<int>(newStatus);
        string newStatusName;

        switch (newStatus) {
            case ProcedureStatus::CheckIn:  newStatusName = "Check in";  break;
            case ProcedureStatus::CheckOut: newStatusName = "Check out"; break;
            case ProcedureStatus::Cancel:   newStatusName = "Cancel";    break;
            case ProcedureStatus::Delay:    throw runtime_error("This function doesn't work with Delay.\n");
            default:                        throw invalid_argument("Invalid status code.");
        }

        for (auto itr = existBooking.begin(); itr != existBooking.end(); ++itr) {
            if (bookingId == itr->id) {
                if (itr->procedure == newStatusName) {
                    throw runtime_error("Reservation is already " + itr->procedure + ".");
                }

                if (itr->procedure == "Cancel" || itr->procedure == "Check out") {
                    throw runtime_error("Cannot update: Reservation is already finalized (" + itr->procedure + ").");
                }

                if (newStatus == ProcedureStatus::CheckOut) {
                    if (itr->procedure != "Check in") {
                        throw runtime_error("Cannot Check out: Reservation must be checked in first (Current status: " + itr->procedure + ").");
                    }
                } else {
                    if (itr->procedure != "Created" && itr->procedure != "Delay") {
                        throw runtime_error("Cannot change to " + newStatusName + " from current status: " + itr->procedure + ".");
                    }
                }

                string sql = "INSERT INTO `Procedure` (reservation_id, procedure_status_id) VALUES (" +
                             to_string(itr->id) + ", " + to_string(statusId) + ");";
                db.execute(sql);
                itr->procedure = newStatusName;
                return;
            }
        }

        throw runtime_error("Reservation not found.\n");
    }

    void delayBooking(int bookingId, const DateTime& newStartTime, const DateTime& newEndTime) {
        for (auto itr = existBooking.begin(); itr != existBooking.end(); ++itr) {
            if (bookingId == itr->id) {
                if (itr->procedure != "Created" && itr->procedure != "Delay") {
                    throw runtime_error("Cannot delay: Reservation is currently: " + itr->procedure + ".");
                }
                Reservation updated = *itr;
                updated.startTime = newStartTime;
                updated.endTime = newEndTime;
                if (hasConflictRoom(updated))
                    throw runtime_error("Conflict detected! Room is not available at the new time.");
                string updateSql = "UPDATE Reservation SET start_time = '" + newStartTime.toMysqlString() +
                                   "', end_time = '" + newEndTime.toMysqlString() +
                                   "' WHERE reservation_id = " + to_string(bookingId) + ";";
                db.execute(updateSql);
                string proSql = "INSERT INTO `Procedure` (reservation_id, procedure_status_id) VALUES (" + to_string(bookingId) + ", 5);";
                db.execute(proSql);
                itr->startTime = newStartTime;
                itr->endTime = newEndTime;
                itr->procedure = "Delay";
                return;
            }
        }
        throw runtime_error("Reservation not found.");
    }

    LinkList<string> viewPopularRoom(int numberOfRoom = 5) {
        string query = "SELECT r.room_code FROM Room r JOIN Reservation res ON r.room_code = res.room_code GROUP BY res.room_code LIMIT " + to_string(numberOfRoom);
        return db.runQuery<string>(query);
    }

    void display() const {
        existBooking.display();
    }
};

// ==========================================
// 8. RAYLIB UI LAYER
// ==========================================
enum AppScreen {
    SCREEN_LOGIN = 0,
    SCREEN_DASHBOARD
};

enum DashboardTab {
    TAB_BOOK_ROOM = 0,
    TAB_MY_BOOKINGS
};

enum LoginBox {
    BOX_NONE = 0,
    BOX_EMAIL,
    BOX_PASSWORD
};

enum StudentInputField {
    FIELD_NONE = 0,
    FIELD_STUDENT_NAME,
    FIELD_STUDENT_ID
};

namespace CampusUI {

enum class RoomType {
    Hall = 0,
    Theory,
    Lab,
    Count
};

enum class BookingMode {
    ByDate = 0,
    BySlot
};

struct UserSession {
    char email[64];
    bool isLoggedIn;
    bool dbConnected;
};

struct RoomCardData {
    int id;
    RoomType type;
    char code[16];
    char name[32];
    int capacity;
    bool isOccupied;
    int selectedDateIdx;
    int selectedSlotIdx;
};

struct BookingRowData {
    bool active;
    int dbReservationId;
    char roomCode[16];
    char roomName[32];
    char date[16];
    char timeSlot[20];
    char studentName[64];
    char studentId[32];
    int status; // 0 = RESERVED, 1 = CHECKED IN
};

constexpr int ROOM_TYPE_COUNT = static_cast<int>(RoomType::Count);
constexpr int ROOM_COUNT = 10;

const char* GetRoomImagePath(RoomType type) {
    if (type == RoomType::Hall) return "Hall.png";
    if (type == RoomType::Theory) return "Theory.png";
    if (type == RoomType::Lab) return "Lab.png";
    return "";
}

RoomType InferRoomTypeFromCode(const char* code) {
    if (code == nullptr || code[0] == '\0') return RoomType::Theory;
    if (code[0] == 'F') return RoomType::Hall;
    if (code[0] == 'E') return RoomType::Theory;
    if (code[0] == 'A') return RoomType::Lab;
    return RoomType::Theory;
}

const char* InferRoomNameFromCode(const char* code, RoomType type) {
    if (code != nullptr) {
        if (strcmp(code, "F101") == 0) return "Cosmo Hall";
        if (strcmp(code, "F202") == 0) return "Jupiter Room";
        if (strcmp(code, "F105") == 0) return "Unicorn Room";
    }
    if (type == RoomType::Hall) return "Hall Space";
    if (type == RoomType::Lab)  return "Lab Class";
    return "Theory Class";
}

void DrawRoomSchematic(int textureIndex, Rectangle box, const Texture2D roomTextures[ROOM_TYPE_COUNT], Font customFont) {
    DrawRectangleRec(box, (Color){ 238, 242, 247, 255 });
    DrawRectangleLinesEx(box, 1.5f, (Color){ 185, 195, 210, 255 });

    if (textureIndex >= 0 && textureIndex < ROOM_TYPE_COUNT && roomTextures[textureIndex].id > 0) {
        Texture2D tex = roomTextures[textureIndex];

        float scaleX = box.width / (float)tex.width;
        float scaleY = box.height / (float)tex.height;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;

        float drawW = tex.width * scale;
        float drawH = tex.height * scale;
        Rectangle dest = {
            box.x + (box.width - drawW) * 0.5f,
            box.y + (box.height - drawH) * 0.5f,
            drawW,
            drawH
        };
        Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };

        DrawTexturePro(tex, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        DrawTextEx(customFont, "No Image", (Vector2){ box.x + 56, box.y + 44 }, 14, 1, GRAY);
    }
}

void BuildDateTimeRange(const char* dateStr, const char* slotStr, DateTime& outStart, DateTime& outEnd) {
    int y = 2026, m = 11, d = 23;
    sscanf(dateStr, "%d-%d-%d", &y, &m, &d);

    int sh = 8, sm = 0, eh = 17, em = 0;
    if (strcmp(slotStr, "All Day") != 0) {
        int eHour12 = 0, eMin = 0;
        if (sscanf(slotStr, "%d:%d - %d:%d", &sh, &sm, &eHour12, &eMin) == 4) {
            eh = eHour12;
            em = eMin;
            if (eh < sh && eh + 12 <= 17) {
                eh += 12;
            }
        }
    }

    outStart = DateTime(y, m, d, sh, sm, 0);
    outEnd   = DateTime(y, m, d, eh, em, 0);
}

AppScreen UpdateAndDrawLoginScreen(Font customFont, UserSession &outUser, Texture2D logoTex, LoginAccount* loginSvc) {
    const int screenWidth = 1000;

    static LoginBox activeBox = BOX_EMAIL;
    static char emailText[64] = "\0";
    static char passwordText[64] = "\0";
    static int emailLetterCount = 0;
    static int passwordLetterCount = 0;
    static char loginError[128] = "\0";

    Rectangle emailBoxBounds = { (float)screenWidth / 2 - 150, 350, 300, 40 };
    Rectangle passwordBoxBounds = { (float)screenWidth / 2 - 150, 430, 300, 40 };
    Rectangle submitButtonBounds = { (float)screenWidth / 2 - 150, 500, 300, 45 };

    Vector2 mousePoint = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePoint, emailBoxBounds)) {
            activeBox = BOX_EMAIL;
        } else if (CheckCollisionPointRec(mousePoint, passwordBoxBounds)) {
            activeBox = BOX_PASSWORD;
        } else {
            activeBox = BOX_NONE;
        }
    }

    if (IsKeyPressed(KEY_TAB)) {
        activeBox = (activeBox == BOX_EMAIL) ? BOX_PASSWORD : BOX_EMAIL;
    }

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125) {
            if (activeBox == BOX_EMAIL && emailLetterCount < 63) {
                emailText[emailLetterCount++] = (char)key;
                emailText[emailLetterCount] = '\0';
            } else if (activeBox == BOX_PASSWORD && passwordLetterCount < 63) {
                passwordText[passwordLetterCount++] = (char)key;
                passwordText[passwordLetterCount] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (activeBox == BOX_EMAIL && emailLetterCount > 0) {
            emailText[--emailLetterCount] = '\0';
        } else if (activeBox == BOX_PASSWORD && passwordLetterCount > 0) {
            passwordText[--passwordLetterCount] = '\0';
        }
    }

    bool isClicked = CheckCollisionPointRec(mousePoint, submitButtonBounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (isClicked || IsKeyPressed(KEY_ENTER)) {
        bool authenticated = false;

        if (outUser.dbConnected && loginSvc != nullptr) {
            try {
                authenticated = loginSvc->login(string(emailText), string(passwordText));
                if (!authenticated) {
                    snprintf(loginError, sizeof(loginError), "Invalid campus email or password.");
                }
            } catch (const exception& ex) {
                snprintf(loginError, sizeof(loginError), "Login error: %s", ex.what());
            }
        } else {
            authenticated = true;
        }

        if (authenticated) {
            loginError[0] = '\0';
            strcpy(outUser.email, emailText);
            outUser.isLoggedIn = true;
            return SCREEN_DASHBOARD;
        }
    }

    if (logoTex.id > 0) {
        float targetWidth = 80.0f;
        float scale = targetWidth / (float)logoTex.width;
        Vector2 pos = { (float)screenWidth / 2.0f - (logoTex.width * scale) / 2.0f, 105.0f };
        DrawTextureEx(logoTex, pos, 0.0f, scale, WHITE);
    }

    DrawTextEx(customFont, "CAMPUS SPACE BOOKER",
               (Vector2){ (float)(screenWidth / 2 - MeasureTextEx(customFont, "CAMPUS SPACE BOOKER", 26, 1).x / 2), 200 },
               26, 1, DARKBLUE);

    DrawTextEx(customFont, "Sign in with your student account",
               (Vector2){ (float)(screenWidth / 2 - MeasureTextEx(customFont, "Sign in with your student account", 16, 1).x / 2), 240 },
               16, 1, GRAY);

    DrawTextEx(customFont, "Campus Email", (Vector2){ emailBoxBounds.x, emailBoxBounds.y - 22 }, 14, 1, DARKGRAY);
    DrawRectangleRec(emailBoxBounds, LIGHTGRAY);
    DrawRectangleLinesEx(emailBoxBounds, 2, (activeBox == BOX_EMAIL) ? BLUE : GRAY);

    if (emailLetterCount == 0 && activeBox != BOX_EMAIL) {
        DrawTextEx(customFont, "student@university.edu", (Vector2){ emailBoxBounds.x + 10, emailBoxBounds.y + 10 }, 16, 1, GRAY);
    } else {
        DrawTextEx(customFont, emailText, (Vector2){ emailBoxBounds.x + 10, emailBoxBounds.y + 10 }, 16, 1, BLACK);
    }

    DrawTextEx(customFont, "Password", (Vector2){ passwordBoxBounds.x, passwordBoxBounds.y - 22 }, 14, 1, DARKGRAY);
    DrawRectangleRec(passwordBoxBounds, LIGHTGRAY);
    DrawRectangleLinesEx(passwordBoxBounds, 2, (activeBox == BOX_PASSWORD) ? BLUE : GRAY);

    if (passwordLetterCount == 0 && activeBox != BOX_PASSWORD) {
        DrawTextEx(customFont, "••••••••", (Vector2){ passwordBoxBounds.x + 10, passwordBoxBounds.y + 10 }, 16, 1, GRAY);
    } else {
        char maskedPassword[64] = "\0";
        for (int i = 0; i < passwordLetterCount; i++) {
            maskedPassword[i] = '*';
        }
        maskedPassword[passwordLetterCount] = '\0';
        DrawTextEx(customFont, maskedPassword, (Vector2){ passwordBoxBounds.x + 10, passwordBoxBounds.y + 12 }, 16, 1, BLACK);
    }

    bool isHovered = CheckCollisionPointRec(mousePoint, submitButtonBounds);
    DrawRectangleRec(submitButtonBounds, isHovered ? BLUE : DARKBLUE);

    Vector2 btnTextSize = MeasureTextEx(customFont, "SIGN IN", 18, 1);
    DrawTextEx(customFont, "SIGN IN",
               (Vector2){ submitButtonBounds.x + (submitButtonBounds.width / 2 - btnTextSize.x / 2), submitButtonBounds.y + 13 },
               18, 1, WHITE);

    if (loginError[0] != '\0') {
        DrawTextEx(customFont, loginError,
                   (Vector2){ (float)(screenWidth / 2 - MeasureTextEx(customFont, loginError, 13, 1).x / 2), 552 },
                   13, 1, RED);
    }

    const char* hintText = outUser.dbConnected
        ? "Press TAB to switch fields | Press ENTER to Submit (DB Connected)"
        : "Press TAB to switch fields | Press ENTER to Submit (Offline Fallback Mode)";
    DrawTextEx(customFont, hintText,
               (Vector2){ (float)(screenWidth / 2 - MeasureTextEx(customFont, hintText, 12, 1).x / 2), 575 },
               12, 1, DARKGRAY);

    return SCREEN_LOGIN;
}

AppScreen UpdateAndDrawDashboard(
    Font customFont,
    UserSession &currentUser,
    const Texture2D roomTextures[ROOM_TYPE_COUNT],
    RoomList* roomListSvc,
    Booking* bookingSvc
) {
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    constexpr int DATE_COUNT = 5;
    constexpr int SLOT_COUNT = 5;
    const char* dates[DATE_COUNT] = { "2026-11-23", "2026-11-24", "2026-11-25", "2026-11-26", "2026-11-27" };
    const char* slots[SLOT_COUNT] = { "08:00 - 9:30", "9:50 - 11:20", "12:10 - 13:40", "13:50 - 15:20", "15:30 - 17:00" };

    static DashboardTab currentTab = TAB_BOOK_ROOM;
    static int selectedCardIdx = 0;
    static char statusBanner[160] = "Pick a Date or Time Slot independently and confirm.";

    static float scrollY = 0.0f;
    const float viewTop = 185.0f;
    const float viewHeight = 755.0f;
    const float scrollSpeed = 45.0f;

    static bool showStudentModal = false;
    static int pendingRoomIdx = -1;
    static BookingMode pendingMode = BookingMode::ByDate;
    static StudentInputField activeField = FIELD_STUDENT_NAME;
    static char studentNameInput[64] = "";
    static char studentIdInput[32] = "";
    static int nameLen = 0;
    static int idLen = 0;

    static bool initializedRooms = false;
    static RoomCardData rooms[ROOM_COUNT] = {
        { 1,  RoomType::Hall,   "F101", "Cosmo Hall",   150, false, 0, 0 },
        { 2,  RoomType::Theory, "E203", "Theory Class",  60, false, 0, 1 },
        { 3,  RoomType::Lab,    "A005", "Lab Class",     35, true,  0, 2 },
        { 4,  RoomType::Hall,   "F202", "Jupiter Room",  75, false, 0, 0 },
        { 5,  RoomType::Theory, "E103", "Theory Class",  60, true,  0, 0 },
        { 6,  RoomType::Theory, "E105", "Theory Class",  60, true,  0, 0 },
        { 7,  RoomType::Lab,    "A103", "Lab Class",     30, true,  0, 0 },
        { 8,  RoomType::Hall,   "F105", "Unicorn Room",  45, true,  0, 0 },
        { 9,  RoomType::Theory, "E113", "Theory Class",  60, true,  0, 0 },
        { 10, RoomType::Lab,    "A207", "Lab Class",     30, true,  0, 0 }
    };

    if (!initializedRooms) {
        if (currentUser.dbConnected && roomListSvc != nullptr) {
            LinkList<Room>& dbRooms = roomListSvc->getRooms();
            int i = 0;
            for (auto itr = dbRooms.begin(); itr != dbRooms.end() && i < ROOM_COUNT; ++itr, ++i) {
                strncpy(rooms[i].code, itr->roomCode.c_str(), sizeof(rooms[i].code) - 1);
                rooms[i].code[sizeof(rooms[i].code) - 1] = '\0';
                rooms[i].capacity = itr->capacity;
                rooms[i].type = InferRoomTypeFromCode(rooms[i].code);
                strncpy(rooms[i].name, InferRoomNameFromCode(rooms[i].code, rooms[i].type), sizeof(rooms[i].name) - 1);
                rooms[i].name[sizeof(rooms[i].name) - 1] = '\0';
                rooms[i].isOccupied = false;
            }
        }
        for (int idx = 0; idx < ROOM_COUNT; idx++) {
            rooms[idx].type = InferRoomTypeFromCode(rooms[idx].code);
        }
        initializedRooms = true;
    }

    static BookingRowData bookings[20] = {};
    static int bookingCount = 0;

    Vector2 mousePoint = GetMousePosition();

    char displayName[64];
    int i = 0;
    while (currentUser.email[i] != '\0' && currentUser.email[i] != '@' && i < 63) {
        displayName[i] = currentUser.email[i];
        i++;
    }
    displayName[i] = '\0';
    if (displayName[0] == '\0') {
        strcpy(displayName, "Student");
    }

    // --- TOP HEADER BAR ---
    DrawRectangle(0, 0, screenWidth, 70, WHITE);
    DrawLine(0, 70, screenWidth, 70, LIGHTGRAY);

    DrawTextEx(customFont, "CAMPUS SPACE BOOKER", (Vector2){ 40, 22 }, 22, 1, DARKBLUE);
    DrawTextEx(customFont, TextFormat("Student: %s", displayName), (Vector2){ 620, 26 }, 15, 1, DARKGRAY);

    Rectangle logoutBtn = { 870, 18, 90, 34 };
    bool hoverLogout = !showStudentModal && CheckCollisionPointRec(mousePoint, logoutBtn);
    DrawRectangleRec(logoutBtn, hoverLogout ? RED : (Color){ 200, 45, 45, 255 });
    DrawTextEx(customFont, "Log Out", (Vector2){ logoutBtn.x + 16, logoutBtn.y + 9 }, 14, 1, WHITE);

    if (!showStudentModal && ((hoverLogout && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ESCAPE))) {
        currentUser.isLoggedIn = false;
        scrollY = 0.0f;
        return SCREEN_LOGIN;
    }

    // --- TAB NAVIGATION ---
    Rectangle tab1Btn = { 40, 90, 180, 38 };
    Rectangle tab2Btn = { 230, 90, 200, 38 };

    if (!showStudentModal) {
        if (CheckCollisionPointRec(mousePoint, tab1Btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (currentTab != TAB_BOOK_ROOM) {
                currentTab = TAB_BOOK_ROOM;
                scrollY = 0.0f;
            }
        }
        if (CheckCollisionPointRec(mousePoint, tab2Btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (currentTab != TAB_MY_BOOKINGS) {
                currentTab = TAB_MY_BOOKINGS;
                scrollY = 0.0f;
            }
        }
    }

    DrawRectangleRec(tab1Btn, (currentTab == TAB_BOOK_ROOM) ? DARKBLUE : LIGHTGRAY);
    DrawTextEx(customFont, "Book Room (Grid)", (Vector2){ tab1Btn.x + 22, tab1Btn.y + 11 }, 15, 1, (currentTab == TAB_BOOK_ROOM) ? WHITE : DARKGRAY);

    DrawRectangleRec(tab2Btn, (currentTab == TAB_MY_BOOKINGS) ? DARKBLUE : LIGHTGRAY);
    DrawTextEx(customFont, TextFormat("My Bookings (%d)", bookingCount), (Vector2){ tab2Btn.x + 28, tab2Btn.y + 11 }, 15, 1, (currentTab == TAB_MY_BOOKINGS) ? WHITE : DARKGRAY);

    // Status Banner
    DrawRectangle(40, 142, 920, 34, (Color){ 232, 240, 254, 255 });
    DrawRectangleLines(40, 142, 920, 34, (Color){ 180, 205, 245, 255 });
    DrawTextEx(customFont, statusBanner, (Vector2){ 55, 151 }, 14, 1, DARKBLUE);

    // --- SCROLL INPUT & CLAMPING ---
    const float cardW = 448.0f;
    const float cardH = 355.0f;
    const float startX = 40.0f;
    const float startY = 195.0f;
    const float gap = 24.0f;

    float contentHeight = 0.0f;
    if (currentTab == TAB_BOOK_ROOM) {
        const int numCols = 2;
        const int numRows = (ROOM_COUNT + numCols - 1) / numCols;
        contentHeight = (numRows > 0) ? (numRows * cardH + (numRows - 1) * gap) : 0.0f;
    } else {
        contentHeight = (bookingCount > 0) ? (bookingCount * 88.0f) : 60.0f;
    }

    if (!showStudentModal) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scrollY -= wheel * scrollSpeed;
        }
    }

    float maxScroll = contentHeight - viewHeight;
    if (maxScroll < 0.0f) maxScroll = 0.0f;
    if (scrollY < 0.0f) scrollY = 0.0f;
    if (scrollY > maxScroll) scrollY = maxScroll;

    Vector2 scrolledMouse = { mousePoint.x, mousePoint.y + scrollY };
    bool insideView = CheckCollisionPointRec(mousePoint, { 0, viewTop, (float)screenWidth, viewHeight });

    BeginScissorMode(0, (int)viewTop, screenWidth, (int)viewHeight);

    // --- TAB 1: ROOM + BOOKING GRID ---
    if (currentTab == TAB_BOOK_ROOM) {
        if (!showStudentModal && IsKeyPressed(KEY_TAB)) {
            selectedCardIdx = (selectedCardIdx + 1) % ROOM_COUNT;
        }

        for (int idx = 0; idx < ROOM_COUNT; idx++) {
            int col = idx % 2;
            int row = idx / 2;
            float cardY = startY + row * (cardH + gap) - scrollY;
            Rectangle card = { startX + col * (cardW + gap), cardY, cardW, cardH };

            if (!showStudentModal && insideView && CheckCollisionPointRec(scrolledMouse, card) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selectedCardIdx = idx;
            }

            DrawRectangleRec(card, WHITE);
            DrawRectangleLinesEx(card, (selectedCardIdx == idx) ? 2.5f : 1.5f, (selectedCardIdx == idx) ? BLUE : LIGHTGRAY);

            DrawTextEx(customFont, TextFormat("%s - %s", rooms[idx].code, rooms[idx].name), (Vector2){ card.x + 16, card.y + 14 }, 16, 1, DARKBLUE);

            Rectangle badge = { card.x + cardW - 108, card.y + 12, 92, 24 };
            DrawRectangleRec(badge, rooms[idx].isOccupied ? (Color){ 254, 226, 226, 255 } : (Color){ 220, 252, 231, 255 });
            DrawTextEx(customFont, rooms[idx].isOccupied ? "OCCUPIED" : "AVAILABLE",
                       (Vector2){ badge.x + 10, badge.y + 5 }, 11, 1,
                       rooms[idx].isOccupied ? RED : DARKGREEN);

            Rectangle picBox = { card.x + 16, card.y + 48, 180, 105 };
            int textureIdx = static_cast<int>(rooms[idx].type);
            DrawRoomSchematic(textureIdx, picBox, roomTextures, customFont);
            DrawTextEx(customFont, TextFormat("Capacity: %d Students", rooms[idx].capacity), (Vector2){ card.x + 16, card.y + 160 }, 13, 1, DARKGRAY);
            DrawTextEx(customFont, "Whiteboard | AC | Wi-Fi", (Vector2){ card.x + 16, card.y + 180 }, 12, 1, GRAY);

            float infoX = card.x + 210;
            DrawTextEx(customFont, "Booking Date:", (Vector2){ infoX, card.y + 46 }, 13, 1, DARKGRAY);
            Rectangle dateBtn = { infoX, card.y + 65, 220, 32 };
            DrawRectangleRec(dateBtn, LIGHTGRAY);
            DrawRectangleLines(dateBtn.x, dateBtn.y, dateBtn.width, dateBtn.height, GRAY);
            DrawTextEx(customFont, TextFormat("%s [Cycle]", dates[rooms[idx].selectedDateIdx]), (Vector2){ dateBtn.x + 10, dateBtn.y + 8 }, 13, 1, BLACK);

            if (!showStudentModal && insideView && CheckCollisionPointRec(scrolledMouse, dateBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                rooms[idx].selectedDateIdx = (rooms[idx].selectedDateIdx + 1) % DATE_COUNT;
            }

            DrawTextEx(customFont, "Time Slot:", (Vector2){ infoX, card.y + 106 }, 13, 1, DARKGRAY);
            Rectangle slotBtn = { infoX, card.y + 125, 220, 32 };
            DrawRectangleRec(slotBtn, LIGHTGRAY);
            DrawRectangleLines(slotBtn.x, slotBtn.y, slotBtn.width, slotBtn.height, GRAY);
            DrawTextEx(customFont, TextFormat("%s [Cycle]", slots[rooms[idx].selectedSlotIdx]), (Vector2){ slotBtn.x + 10, slotBtn.y + 8 }, 13, 1, BLACK);

            if (!showStudentModal && insideView && CheckCollisionPointRec(scrolledMouse, slotBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                rooms[idx].selectedSlotIdx = (rooms[idx].selectedSlotIdx + 1) % SLOT_COUNT;
            }

            bool canBook = !rooms[idx].isOccupied;
            Rectangle bookDateBtn = { card.x + 16, card.y + cardH - 54, (cardW - 44) * 0.5f, 38 };
            Rectangle bookSlotBtn = { card.x + 16 + (cardW - 44) * 0.5f + 12, card.y + cardH - 54, (cardW - 44) * 0.5f, 38 };

            bool hoverDateBook = !showStudentModal && insideView && CheckCollisionPointRec(scrolledMouse, bookDateBtn);
            bool hoverSlotBook = !showStudentModal && insideView && CheckCollisionPointRec(scrolledMouse, bookSlotBtn);

            DrawRectangleRec(bookDateBtn, canBook ? (hoverDateBook ? BLUE : DARKBLUE) : GRAY);
            const char* dateLabel = canBook ? "Book Date" : "Occupied";
            Vector2 dLblSize = MeasureTextEx(customFont, dateLabel, 14, 1);
            DrawTextEx(customFont, dateLabel,
                       (Vector2){ bookDateBtn.x + (bookDateBtn.width / 2 - dLblSize.x / 2), bookDateBtn.y + 12 },
                       14, 1, WHITE);

            DrawRectangleRec(bookSlotBtn, canBook ? (hoverSlotBook ? BLUE : (Color){ 30, 90, 150, 255 }) : GRAY);
            const char* slotLabel = canBook ? "Book Slot" : "Occupied";
            Vector2 sLblSize = MeasureTextEx(customFont, slotLabel, 14, 1);
            DrawTextEx(customFont, slotLabel,
                       (Vector2){ bookSlotBtn.x + (bookSlotBtn.width / 2 - sLblSize.x / 2), bookSlotBtn.y + 12 },
                       14, 1, WHITE);

            bool triggerDateBook = !showStudentModal && (
                (canBook && hoverDateBook && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ||
                (canBook && selectedCardIdx == idx && IsKeyPressed(KEY_ENTER))
            );
            bool triggerSlotBook = !showStudentModal && canBook && hoverSlotBook && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            if ((triggerDateBook || triggerSlotBook) && bookingCount < 20) {
                pendingRoomIdx = idx;
                pendingMode = triggerSlotBook ? BookingMode::BySlot : BookingMode::ByDate;
                showStudentModal = true;
                activeField = FIELD_STUDENT_NAME;

                strcpy(studentNameInput, displayName);
                nameLen = (int)strlen(studentNameInput);
                studentIdInput[0] = '\0';
                idLen = 0;
            }
        }
    }
    // --- TAB 2: MY BOOKINGS ---
    else {
        float rowY = 195.0f;

        if (bookingCount == 0) {
            DrawTextEx(customFont, "No active reservations. Switch to 'Book Room (Grid)' to reserve a space.",
                       (Vector2){ 40, 220.0f - scrollY }, 16, 1, GRAY);
        }

        for (int b = 0; b < bookingCount; b++) {
            float drawY = rowY - scrollY;
            Rectangle rowBox = { 40, drawY, 920, 74 };
            DrawRectangleRec(rowBox, WHITE);
            DrawRectangleLinesEx(rowBox, 1.5f, LIGHTGRAY);

            DrawTextEx(customFont, TextFormat("%s - %s", bookings[b].roomCode, bookings[b].roomName), (Vector2){ rowBox.x + 20, rowBox.y + 14 }, 17, 1, DARKBLUE);
            DrawTextEx(customFont, TextFormat("Date: %s | Slot: %s | %s (%s)",
                                             bookings[b].date, bookings[b].timeSlot,
                                             bookings[b].studentName, bookings[b].studentId),
                       (Vector2){ rowBox.x + 20, rowBox.y + 42 }, 14, 1, DARKGRAY);

            Rectangle stPill = { rowBox.x + 480, rowBox.y + 22, 120, 30 };
            DrawRectangleRec(stPill, (bookings[b].status == 0) ? (Color){ 254, 243, 199, 255 } : (Color){ 220, 252, 231, 255 });
            DrawTextEx(customFont, (bookings[b].status == 0) ? "RESERVED" : "CHECKED IN",
                       (Vector2){ stPill.x + 14, stPill.y + 8 }, 13, 1,
                       (bookings[b].status == 0) ? ORANGE : DARKGREEN);

            Rectangle actionBtn = { rowBox.x + 620, rowBox.y + 19, 140, 36 };
            bool hoverAct = !showStudentModal && insideView && CheckCollisionPointRec(scrolledMouse, actionBtn);
            DrawRectangleRec(actionBtn, hoverAct ? BLUE : DARKBLUE);
            DrawTextEx(customFont, (bookings[b].status == 0) ? "Check In" : "Check Out",
                       (Vector2){ actionBtn.x + 32, actionBtn.y + 10 }, 14, 1, WHITE);

            if (hoverAct && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (bookings[b].status == 0) {
                    if (currentUser.dbConnected && bookingSvc != nullptr && bookings[b].dbReservationId > 0) {
                        try {
                            bookingSvc->updateBookingStatus(bookings[b].dbReservationId, ProcedureStatus::CheckIn);
                        } catch (const exception& ex) {
                            snprintf(statusBanner, sizeof(statusBanner), "%s", ex.what());
                            continue;
                        }
                    }
                    bookings[b].status = 1;
                    snprintf(statusBanner, sizeof(statusBanner), "Checked in to %s.", bookings[b].roomCode);
                } else {
                    if (currentUser.dbConnected && bookingSvc != nullptr && bookings[b].dbReservationId > 0) {
                        try {
                            bookingSvc->updateBookingStatus(bookings[b].dbReservationId, ProcedureStatus::CheckOut);
                        } catch (const exception& ex) {
                            snprintf(statusBanner, sizeof(statusBanner), "%s", ex.what());
                            continue;
                        }
                    }
                    for (int r = 0; r < ROOM_COUNT; r++) {
                        if (strcmp(rooms[r].code, bookings[b].roomCode) == 0) rooms[r].isOccupied = false;
                    }
                    snprintf(statusBanner, sizeof(statusBanner), "Checked out of %s. Room is now available.", bookings[b].roomCode);
                    for (int k = b; k < bookingCount - 1; k++) bookings[k] = bookings[k + 1];
                    bookingCount--;
                    b--;
                    continue;
                }
            }

            Rectangle cancelBtn = { rowBox.x + 775, rowBox.y + 19, 125, 36 };
            bool hoverCancel = !showStudentModal && insideView && CheckCollisionPointRec(scrolledMouse, cancelBtn);
            DrawRectangleRec(cancelBtn, hoverCancel ? RED : (Color){ 200, 45, 45, 255 });
            DrawTextEx(customFont, "Cancel", (Vector2){ cancelBtn.x + 36, cancelBtn.y + 10 }, 14, 1, WHITE);

            if (hoverCancel && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (currentUser.dbConnected && bookingSvc != nullptr && bookings[b].dbReservationId > 0) {
                    try {
                        bookingSvc->updateBookingStatus(bookings[b].dbReservationId, ProcedureStatus::Cancel);
                    } catch (const exception& ex) {
                        snprintf(statusBanner, sizeof(statusBanner), "%s", ex.what());
                        continue;
                    }
                }
                for (int r = 0; r < ROOM_COUNT; r++) {
                    if (strcmp(rooms[r].code, bookings[b].roomCode) == 0) rooms[r].isOccupied = false;
                }
                snprintf(statusBanner, sizeof(statusBanner), "Cancelled booking for %s.", bookings[b].roomCode);
                for (int k = b; k < bookingCount - 1; k++) bookings[k] = bookings[k + 1];
                bookingCount--;
                b--;
                continue;
            }

            rowY += 88.0f;
        }
    }

    EndScissorMode();

    // --- SCROLLBAR INDICATOR ---
    if (maxScroll > 0.0f) {
        float trackX = 972.0f;
        float trackY = viewTop;
        float trackW = 10.0f;
        float trackH = viewHeight;

        float thumbH = (viewHeight / contentHeight) * trackH;
        if (thumbH < 30.0f) thumbH = 30.0f;
        float thumbY = trackY + (scrollY / maxScroll) * (trackH - thumbH);

        DrawRectangleRounded({ trackX, trackY, trackW, trackH }, 0.4f, 6, LIGHTGRAY);
        DrawRectangleRounded({ trackX, thumbY, trackW, thumbH }, 0.4f, 6, DARKGRAY);
    }

    // --- STUDENT INFO MODAL OVERLAY ---
    if (showStudentModal && pendingRoomIdx >= 0 && pendingRoomIdx < ROOM_COUNT) {
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){ 0, 0, 0, 120 });

        Rectangle modal = { 260, 290, 480, 320 };
        DrawRectangleRec(modal, WHITE);
        DrawRectangleLinesEx(modal, 2.0f, DARKBLUE);

        const char* selectionSummary = (pendingMode == BookingMode::ByDate)
            ? dates[rooms[pendingRoomIdx].selectedDateIdx]
            : slots[rooms[pendingRoomIdx].selectedSlotIdx];

        DrawTextEx(customFont,
                   TextFormat("Student Info - %s (%s)", rooms[pendingRoomIdx].code, selectionSummary),
                   (Vector2){ modal.x + 24, modal.y + 20 }, 18, 1, DARKBLUE);

        Rectangle nameBox = { modal.x + 24, modal.y + 85, 432, 38 };
        Rectangle idBox   = { modal.x + 24, modal.y + 160, 432, 38 };
        Rectangle confirmModalBtn = { modal.x + 24, modal.y + 235, 206, 44 };
        Rectangle cancelModalBtn  = { modal.x + 250, modal.y + 235, 206, 44 };

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePoint, nameBox)) activeField = FIELD_STUDENT_NAME;
            else if (CheckCollisionPointRec(mousePoint, idBox)) activeField = FIELD_STUDENT_ID;
        }
        if (IsKeyPressed(KEY_TAB)) {
            activeField = (activeField == FIELD_STUDENT_NAME) ? FIELD_STUDENT_ID : FIELD_STUDENT_NAME;
        }

        int ch = GetCharPressed();
        while (ch > 0) {
            if (ch >= 32 && ch <= 125) {
                if (activeField == FIELD_STUDENT_NAME && nameLen < 63) {
                    studentNameInput[nameLen++] = (char)ch;
                    studentNameInput[nameLen] = '\0';
                } else if (activeField == FIELD_STUDENT_ID && idLen < 31) {
                    studentIdInput[idLen++] = (char)ch;
                    studentIdInput[idLen] = '\0';
                }
            }
            ch = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (activeField == FIELD_STUDENT_NAME && nameLen > 0) {
                studentNameInput[--nameLen] = '\0';
            } else if (activeField == FIELD_STUDENT_ID && idLen > 0) {
                studentIdInput[--idLen] = '\0';
            }
        }

        DrawTextEx(customFont, "Student Full Name:", (Vector2){ nameBox.x, nameBox.y - 22 }, 14, 1, DARKGRAY);
        DrawRectangleRec(nameBox, LIGHTGRAY);
        DrawRectangleLinesEx(nameBox, 2, (activeField == FIELD_STUDENT_NAME) ? BLUE : GRAY);
        DrawTextEx(customFont, studentNameInput, (Vector2){ nameBox.x + 10, nameBox.y + 10 }, 15, 1, BLACK);

        DrawTextEx(customFont, "Student ID:", (Vector2){ idBox.x, idBox.y - 22 }, 14, 1, DARKGRAY);
        DrawRectangleRec(idBox, LIGHTGRAY);
        DrawRectangleLinesEx(idBox, 2, (activeField == FIELD_STUDENT_ID) ? BLUE : GRAY);
        DrawTextEx(customFont, studentIdInput, (Vector2){ idBox.x + 10, idBox.y + 10 }, 15, 1, BLACK);

        DrawRectangleRec(confirmModalBtn, DARKBLUE);
        DrawTextEx(customFont, "Submit Booking", (Vector2){ confirmModalBtn.x + 42, confirmModalBtn.y + 12 }, 15, 1, WHITE);

        DrawRectangleRec(cancelModalBtn, (Color){ 200, 45, 45, 255 });
        DrawTextEx(customFont, "Cancel", (Vector2){ cancelModalBtn.x + 76, cancelModalBtn.y + 12 }, 15, 1, WHITE);

        bool submitted = CheckCollisionPointRec(mousePoint, confirmModalBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool cancelled = CheckCollisionPointRec(mousePoint, cancelModalBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (submitted) {
            if (nameLen == 0 || idLen == 0) {
                snprintf(statusBanner, sizeof(statusBanner), "Please enter both Student Name and Student ID.");
            } else {
                BookingRowData newRow = {};
                newRow.active = true;
                newRow.dbReservationId = 0;
                strcpy(newRow.roomCode, rooms[pendingRoomIdx].code);
                strcpy(newRow.roomName, rooms[pendingRoomIdx].name);

                if (pendingMode == BookingMode::ByDate) {
                    strcpy(newRow.date, dates[rooms[pendingRoomIdx].selectedDateIdx]);
                    strcpy(newRow.timeSlot, "All Day");
                } else {
                    strcpy(newRow.date, dates[rooms[pendingRoomIdx].selectedDateIdx]);
                    strcpy(newRow.timeSlot, slots[rooms[pendingRoomIdx].selectedSlotIdx]);
                }

                strcpy(newRow.studentName, studentNameInput);
                strcpy(newRow.studentId, studentIdInput);
                newRow.status = 0;

                if (currentUser.dbConnected && bookingSvc != nullptr) {
                    try {
                        Reservation res;
                        BuildDateTimeRange(newRow.date, newRow.timeSlot, res.startTime, res.endTime);
                        res.purpose = string("Campus Room Booking by ") + studentNameInput;
                        res.roomCode = string(rooms[pendingRoomIdx].code);
                        res.studentId = atoi(studentIdInput);
                        bookingSvc->addBooking(res);
                        newRow.dbReservationId = res.id;
                    } catch (const exception& ex) {
                        snprintf(statusBanner, sizeof(statusBanner), "%s", ex.what());
                        showStudentModal = false;
                        pendingRoomIdx = -1;
                        return SCREEN_DASHBOARD;
                    }
                }

                bookings[bookingCount++] = newRow;
                rooms[pendingRoomIdx].isOccupied = true;
                snprintf(statusBanner, sizeof(statusBanner), "Booked %s (%s) for %s!",
                         rooms[pendingRoomIdx].code, selectionSummary, studentNameInput);

                showStudentModal = false;
                pendingRoomIdx = -1;
            }
        } else if (cancelled) {
            showStudentModal = false;
            pendingRoomIdx = -1;
        }
    }

    DrawTextEx(customFont, "Shortcuts: [TAB] Cycle Card | [ENTER] Book Date | [ESC] Log Out | [Wheel] Scroll",
               (Vector2){ 40, 960 }, 13, 1, DARKGRAY);

    return SCREEN_DASHBOARD;
}

} // namespace CampusUI

// ==========================================
// 9. MAIN ENTRY POINT
// ==========================================
int main() {
    const int screenWidth = 1000;
    const int screenHeight = 1000;

    InitWindow(screenWidth, screenHeight, "Smart Campus App");

    Font customFont = LoadFontEx("Poppins-BlackItalic.ttf", 32, 0, 250);
    SetTextureFilter(customFont.texture, TEXTURE_FILTER_BILINEAR);

    Texture2D loginLogo = LoadTexture("assets/campus_logo.png");
    if (loginLogo.id > 0) {
        SetTextureFilter(loginLogo, TEXTURE_FILTER_BILINEAR);
    }

    Texture2D roomTextures[CampusUI::ROOM_TYPE_COUNT];
    for (int r = 0; r < CampusUI::ROOM_TYPE_COUNT; r++) {
        CampusUI::RoomType type = static_cast<CampusUI::RoomType>(r);
        roomTextures[r] = LoadTexture(CampusUI::GetRoomImagePath(type));
        if (roomTextures[r].id > 0) {
            SetTextureFilter(roomTextures[r], TEXTURE_FILTER_BILINEAR);
        }
    }

    Database* dbPtr = nullptr;
    LoginAccount* loginPtr = nullptr;
    RoomList* roomListPtr = nullptr;
    Booking* bookingPtr = nullptr;
    bool dbConnected = false;

    try {
        map<string, string> env = load_env(".env");
        string host = env.count("DB_HOST") ? env["DB_HOST"] : "127.0.0.1";
        string user = env.count("DB_USER") ? env["DB_USER"] : "root";
        string pass = env.count("DB_PASS") ? env["DB_PASS"] : "";
        string name = env.count("DB_NAME") ? env["DB_NAME"] : "campus_booking";
        int port    = env.count("DB_PORT") ? stoi(env["DB_PORT"]) : 3306;

        dbPtr = new Database(host.c_str(), user.c_str(), pass.c_str(), name.c_str(), port);
        loginPtr = new LoginAccount(*dbPtr);
        roomListPtr = new RoomList(*dbPtr);
        bookingPtr = new Booking(*dbPtr);
        dbConnected = true;
    } catch (const exception& ex) {
        cerr << "Database connection warning: " << ex.what() << endl;
        dbConnected = false;
    }

    AppScreen currentScreen = SCREEN_LOGIN;
    CampusUI::UserSession currentUser = { "", false, dbConnected };

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){ 243, 244, 246, 255 });

        if (currentScreen == SCREEN_LOGIN) {
            currentScreen = CampusUI::UpdateAndDrawLoginScreen(customFont, currentUser, loginLogo, loginPtr);
        } else if (currentScreen == SCREEN_DASHBOARD) {
            currentScreen = CampusUI::UpdateAndDrawDashboard(customFont, currentUser, roomTextures, roomListPtr, bookingPtr);
        }

        EndDrawing();
    }

    delete bookingPtr;
    delete roomListPtr;
    delete loginPtr;
    delete dbPtr;

    for (int r = 0; r < CampusUI::ROOM_TYPE_COUNT; r++) {
        UnloadTexture(roomTextures[r]);
    }
    UnloadTexture(loginLogo);
    UnloadFont(customFont);
    CloseWindow();

    return 0;
}
