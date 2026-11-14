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
using namespace std;

template <typename T>
T inputVariable(const string messages);
void pressEnterToContinue();

void displayMenu(){
    cout << "1. Booking room.\n";
    cout << "2. Cancel reservation.\n";
    cout << "3. Check in.\n";
    cout << "4. Check out.\n";
    cout << "5. Search Room.\n";
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
    int choice;
    do{
        displayMenu();
        choice = inputVariable<int>("Enter your choice: ");
        switch(choice){
        case 1:{
            Booking b(db);
            b.display();
            pressEnterToContinue();
            clearScreen();
            break;
        }
        default:
            cout << "Invalid input.\n";
            break;
        }
    }while(choice != 7);


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
