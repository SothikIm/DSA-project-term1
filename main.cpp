#include <iostream>
#include <windows.h>
#include <mariadb/mysql.h>
#include <cstdlib>
#include <conio.h>
#include<map>
#include<vector>
#include<string>
#include "load_env.hpp"
#include "linked_list.hpp"
#include "struct.hpp"
#include "database.hpp"
#include "booking.hpp"
#include "login.hpp"
using namespace std;

template <typename T>
T inputVariable(const string messages);
void pressEnterToContinue();

void displayMenu(){
    cout << "1. Booking room.\n";
    cout << "2. Cancel reservation.\n";
    cout << "3. Check in.\n";
    cout << "4. Check out.\n";
    cout << "5. Delay booking.\n";
    cout << "6. List top active rooms.\n";
    cout << "7. Exit.\n";
}

void clearScreen(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main() {
    // load env
    auto env = load_env();
    // connect to MySQL Database
    Database db(env["HOST"].c_str(),
                env["USER"].c_str(),
                env["PASSWORD"].c_str(),
                env["DB"].c_str(),
                stoi(env["PORT"]));
    LoginAccount acc(db);
    acc.displayAllAccount();
    Booking b(db);
    int choice;
    Reservation res;
    do{
        displayMenu();
        choice = inputVariable<int>("Enter your choice: ");
        switch(choice){
        case 1:{
            b.display();
            cin >> res;
            try{
                b.addBooking(res);
                cout << "Add booking successfully.\n";
            }
            catch(const runtime_error& e){
                cerr << e.what() << endl;
            }
            pressEnterToContinue();
            clearScreen();
            break;
        }
        case 2:{
            b.display();
            int reservationId = inputVariable<int>("Enter your reservation ID.");
            try{
                b.updateBookingStatus(reservationId, ProcedureStatus::Cancel);
                cout << "Cancel successfully.\n";
            }
            catch(const runtime_error& err){
                cerr << err.what() << endl;
            }
            pressEnterToContinue();
            clearScreen();
            break;
        }
        case 3:{
            b.display();
            int reservationId = inputVariable<int>("Enter your reservation ID: ");
            try{
                b.updateBookingStatus(reservationId, ProcedureStatus::CheckIn);
                cout << "Check in successfully.\n";
            }
            catch(const runtime_error& err){
                cerr << err.what() << endl;
            }
            pressEnterToContinue();
            clearScreen();
            break;
        }
        case 4:{
            b.display();
            int reservationId = inputVariable<int>("Enter your reservation ID: ");
            try{
                b.updateBookingStatus(reservationId, ProcedureStatus::CheckOut);
                cout << "Check out successfully.\n";
            }
            catch(const runtime_error& err){
                cerr << err.what() << endl;
            }
            pressEnterToContinue();
            clearScreen();
            break;
        }
        case 5:{
            b.display();
            int reservationId = inputVariable<int>("Enter your reservation ID: ");
            DateTime newStartTime = inputVariable<DateTime>("Enter new start time: ");
            DateTime newEndTime = inputVariable<DateTime>("Enter new end time: ");
            try{
                b.delayBooking(reservationId, newStartTime, newEndTime);
                cout << "Delay successfully.\n";
            }
            catch(const runtime_error& err){
                cerr << err.what() << endl;
            }
            pressEnterToContinue();
            clearScreen();
            break;
        }
        case 6:{
            int numberOfRoom = inputVariable<int>("Enter number of rooms you want to views (only 70 rooms in the school): ");
            LinkList<string> popularRoom = b.viewPopulaorRoom(numberOfRoom);
            popularRoom.display();
            break;
        }
        case 7:{
            cout << "System Exit.\n";
        }
        default:
            cout << "Invalid input.\n";
            break;
        }
    }while(choice != 8);


    return 0;
}

template <typename T>
T inputVariable(const string messages){
    T var;
    while(true)
    {
        try{
            cout << messages;
            // constexpr means “this can be evaluated at compile time.
            if constexpr (is_same_v<T, string>){
                // ws mean remove whitespace in input buffer
                getline(cin >> ws, var);
                while (!var.empty() && var.back() == ' ')
                {
                    var.pop_back();
                }
            }
            else{
                if(!(cin >> var)){
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    throw runtime_error("Invalid Input");
                }
            }
            break;
        }
        catch (runtime_error& e){
            cerr << e.what() << endl;
        }
    }
    return var;
}

void pressEnterToContinue(){
    cout << "Press Enter to continue: ";
    while(true){
        if(_kbhit()){ // check whether key is press or not
            char key = _getch(); // get the key without input
            if(key == '\r'){
                break;
            }
        }
    }
}
