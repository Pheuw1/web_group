#include "webserv.hpp"
#include <iostream>
using namespace std;

int main(int ac, char **av, char **env) {
    if (ac < 2) {cerr << "require a single configuration file\n"; return 1;}
    try {
        WebServ serv(av[1], env);
        serv.run();
    } catch (exception &e) {cerr << e.what() << endl;}
    
    return 0;
}