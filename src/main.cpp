#include "webserv.hpp"
#include <iostream>
using namespace std;

int main(int ac, char **av, char **env) {
    try {
        if (ac > 2) {throw invalid_argument("require one or no argument : path to config");}
        string path = "./config/webserv.conf";
        if (av[1])  {path = av[1];}
        WebServ serv(path, env);
        serv.run();
    } catch (exception &e) {cerr << e.what() << endl;}
    
    return 0;
}