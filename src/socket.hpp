#pragma once
#include "http.hpp"
#include "webserv.hpp"
using namespace std;

class Socket {
private:
    typedef vector<int>::iterator IT;
    struct sockaddr_in address;
    char            buffer[BUFF_SIZE];
public:
    vector<int>     c_sd;
    int             master_socket;
    int             port;
    Socket(int port) ;
    ~Socket() {};
    int     check_ready(fd_set &readfds, fd_set &writefds);//returns max_fd for select // not necessary for epoll?
    void    new_connection(fd_set &readfds, fd_set &writefds) ;
    int     messages(fd_set &readfds, WebServ *w);
    class socket_except :  public exception {
    public:
        const char* what() const throw() {
            return ("Couldn't initialize socket\n");
        }
    };

    class connect_except : public exception {
    public:
        const char* what() const throw() {
            return ("Couldn't connect to client socket\n");
        }
    };
};

