#include "server.hpp"
#include "http.hpp"
using namespace std;

Server::Server(vector<vector<string> > settings, vector<vector<string> > routes, vector<vector<string> > dir, WebServ *web)
: w(web), error_path(web->root + "errors/"), max_body_size(0), root(web->cwd + "/HTML/")  {
    string tmp[] = {"listen" ,"server_name"};//this could mandatory ones
    cout << "new server " << endl;
    vector <string> params(tmp, tmp + 2);//
    for (size_t i = 0; i < settings.size(); i++) {
        if (settings[i][0] == "listen") {
            if (settings[i].size() < 2) ports.push_back(80);//default
            for (size_t j = 1; j < settings[i].size(); j++)
                ports.push_back(strtol(settings[i][j].data(),NULL,10)); 
        }
        else if (settings[i][0] == "server_name") {
            if (settings[i].size() < 2) names.push_back("");
            names = vector<string>(settings[i].begin() + 1, settings[i].end());
        }
        else if (settings[i][0] == "cgi")
            cgi_ext = vector<string>(settings[i].begin() + 1, settings[i].end());
        else if (settings[i][0] == "max_body_size")
            max_body_size = atoi(settings[i][1].data());
        else if (settings[i][0] == "errors")
            error_path = settings[i][1];
        std::vector<std::string>::iterator v = find(params.begin(),params.end(),settings[i][0]);
        if (v != params.end())
            params.erase(v);//
    }
    if (!params.empty()) throw invalid_argument("config doesnt have required declarations");
    for (size_t i = 0; i < routes.size(); i++) 
        route_methods.insert(make_pair(routes[i][0], vector<string>(routes[i].begin() + 1, routes[i].end())));
    for (size_t i = 0; i < dir.size(); i++)
        dirs.insert(make_pair(dir[i][0] , dir[i][1]));
    for (size_t i = 0; i < ports.size(); i++)
        sockets.push_back(Socket(ports[i]));

}

int Server::check_ready(fd_set &readfds, fd_set &writefds) {
        int max_fd = 0;  
        for (IT it = sockets.begin(); it != sockets.end(); it++)
            max_fd = max(max_fd, it->master_socket);
        for (IT it = sockets.begin(); it != sockets.end(); it++)
            max_fd = max(max_fd, it->check_ready(readfds, writefds));
        return max_fd;
}  

int Server::get_requests(fd_set &readfds, fd_set &writefds, WebServ *w) {
    for (IT it = sockets.begin(); it != sockets.end(); it++) {
        it->new_connection(readfds, writefds);
        it->messages(readfds, w);
    }
    return 0;
}   


// int Server::serve_response(Response &rep, fd_set &writefds, WebServ *w)
// {
//     for (IT it = sockets.begin(); it != sockets.end(); it++) {
//         getpeername(*it , (struct sockaddr*)&address , (socklen_t*)&addrlen);//something for sd to work properly?
//         if (FD_ISSET(*it , &writefds))
//             if (send(*it, rep.buffer.str().data(), rep.buffer.str().size(), 0) <= 0) {close(*it); c_sd.erase(it--);}
//         // cout << "TO PORT :" << ntohs(address.sin_port) << rep.buffer.str() << endl;
//     }
//     return 0;            // throw runtime_error("send failed\n");
// }


Server::~Server() {
        for (IT it = sockets.begin(); it != sockets.end(); it++)
            for (size_t i = 0; i < it->c_sd.size(); i++)
                close(it->c_sd[i]);            
}