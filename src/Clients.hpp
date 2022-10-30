#include "webserv.hpp"

typedef map<string,string>::iterator IT;

class Client{
public:
    Client(int sd);
    int sd;
    map<string,string> cookies;//name|value
    string Set_cookies(ofstream header) {
        for (IT it = cookies.begin(); it != cookies.end(); it++)
    }
    void add_cookie(string name, string val) {
        cookies.insert(make_pair(name, val));
    }
    //save cookie to file// brwoser does that 
};