#ifndef LOAD_ENV
#define LOAD_ENV
#include<fstream>
#include<iostream>
#include<map>
#include<string>
using namespace std;

string trim(string str){
    size_t first = str.find_first_not_of(" \t\r\n\"'");
    if(first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n\"'");
    return str.substr(first, (last - first + 1));
}

map<string, string> load_env(const string& filepath = ".env"){
    map<string, string> env;
    ifstream ifs(filepath);
    if(!ifs.is_open()){
        cerr << "Can't open file\n";
        return env;
    }
    string line;
    while(getline(ifs >> ws, line)){
        string trimmed_line = trim(line);
        if (trimmed_line.empty() || trimmed_line[0] == '#') continue;
        size_t eq_pos = line.find('=');
        if(eq_pos != string::npos){
            string key = trim(line.substr(0, eq_pos));
            string value = trim(line.substr(eq_pos + 1));
            env[key] = value;
        }
    }
    return env;
}

#endif