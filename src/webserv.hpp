#pragma once
#define MAX_CONNECTION 128
#define TIMEOUT 5000
#define BUFF_SIZE 4096
#include <stdio.h> 
#include <cstring>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <vector>    
#include <algorithm>
#include <locale>   
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/select.h>
# include <signal.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <ostream>
# include <map>
#include <set>
#include <dirent.h>
#ifdef __linux__
# include <wait.h>
#endif
//~~~~~~~~~~~~~~~~~~~~//
/*• Le premier serveur pour un host:port sera le serveur par défaut pour cet host:port
(ce qui signifie qu’il répondra à toutes les requêtes qui n’appartiennent pas à un
autre serveur).*/
class Response;
class Request;
typedef int (*Method) (Request &, Response &);
using namespace std;
class Server;
class WebServ {
public:
    //config

    map<string, Method> Methods;
    map<int, string> HttpStatusCode;
    set<string> cgi_exts;
    //the stuff
    fd_set              readfds;  
    fd_set              writefds;  
    int                 max_sd;
    vector<Server>      servers;
    vector<Request>     requests;
    vector<Response>    responses;
    string              error_path;
    string              root; 
    string              home;
    WebServ(string config_path, char **env);
    ~WebServ();
    void run();
    // vector<Response> rep_by_host(WebServ *w);
    Server *get_host(string name);
    //stupid
    void init(char **env);
    char * const *env;
    string cwd;
};
#define NAME ""
#define MAX_BODY 0
#define LISTEN 80
#define ERROR "./HTML/error/" 
#define ROOT "./"
# include "socket.hpp"
# include "parse.hpp"
# include "http.hpp"
# include "server.hpp"
