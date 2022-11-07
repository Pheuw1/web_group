#pragma once
#include "webserv.hpp"
#include "socket.hpp"
//-------------default values-----------//

using namespace std;
class Socket;
class Server
{
private:
    vector<vector<string> > settings;
    //

public:
    vector<int>     ports;
    vector<Socket>  sockets;
    ~Server();
    typedef vector<Socket>::iterator IT;
    WebServ         *w;
    string          error_path;
    vector<string>  cgi_ext;
    int             max_body_size;//error if too long 413? content-length in request
    string          root; //also this to manage from config
    map<string, vector<string> > route_methods;
    map<string, string> dirs;
    vector<string>  names;
    Server(vector<vector<string> > settings, vector<vector<string> > r, vector<vector<string> > d, WebServ *web);
    int check_ready(fd_set &readfds, fd_set &writefds);
    int get_requests(fd_set &readfds, fd_set &writefds, WebServ *w);
    int serve_response(Response &rep, fd_set &writefds, WebServ *w);
    Server(const Server &);
};
