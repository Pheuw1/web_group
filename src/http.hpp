#pragma once
#include "webserv.hpp"
#include "server.hpp"
using namespace std;

class Request {
public:
    WebServ &w;//used for http code lookup and method implementation
    string version;
    string method_name; 
    string url;
    string header;
    string body;
    Server *host;
    int     sd;
    string obuff;
    int     port;
    string bound;
    Request(char *buffer, WebServ *web, int sd, int port);
    ~Request(); 
    vector <string> get_val(string key);
    vector <string> get_val(string, string key);
    Request &operator=(const Request &req) {
        w = req.w;
        version = req.version;
        method_name = req.method_name;
        url = req.url;
        header = req.header;
        body = req.body;
        host = req.host;
        sd = req.sd;
        port = req.port;
        bound = req.bound;
        return *this;
    }
};

class Response{
public:
    WebServ &w;//used for http code lookup and method implementation
    string version;
    int     status;
    string description;
    //start line
    string headers;
    //empty line
    stringstream body;   
    stringstream buffer;
    Response(const Request &req);
    Response(const Response &rep): w(rep.w), req_cp((char *)rep.req_cp.obuff.data(), &rep.req_cp.w, (const int)rep.req_cp.sd, rep.req_cp.port) {
        version = rep.version;
        status = rep.status;
        description = rep.description;
        headers = rep.headers;
        body << rep.body.str();
        buffer << rep.buffer.str();
    }
    ~Response();
    Request req_cp;
    Response &operator=(const Response &rep) {
        w = rep.w;
        version = rep.version;
        status = rep.status;
        description = rep.description;
        headers = rep.headers;
        body << rep.body.str();
        buffer << rep.buffer.str();
        req_cp = rep.req_cp;
        return *this;
    }

};




int GET(Request &req, Response &rep);
int DELETE(Request &req, Response &rep);
int POST(Request &req, Response &rep);
int CGI(Request &req, Response &rep);
int autoindex(string dir, Response &rep);
