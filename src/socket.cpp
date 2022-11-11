#include "http.hpp"
#include "webserv.hpp"
using namespace std;

Socket::Socket(int port) :port(port){
    int opt = true;
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;//hostaname here ?
    address.sin_port = htons(port); 
    if ((!(master_socket = socket(AF_INET , SOCK_STREAM , 0)) ) || (setsockopt(master_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0 ))
        throw socket_except();
    if  ((bind(master_socket, (struct sockaddr *)   &address, sizeof(address)) < 0) || (listen(master_socket, MAX_CONNECTION)))
        throw connect_except();
    cout << "Listener on port :" <<  port << endl;
}

int Socket::check_ready(fd_set &readfds, fd_set &writefds) {
    int max_sd;
    
    FD_SET(master_socket, &readfds); FD_SET(master_socket, &writefds);  
    max_sd = master_socket;
    for (IT it = c_sd.begin() ; it != c_sd.end() ; it++)  {
            if (*it > 0)  
                FD_SET(*it , &readfds);  FD_SET(*it , &writefds);
            max_sd = max(max_sd, *it);
    }
    return max_sd;
}
    
void Socket::new_connection(fd_set &readfds, fd_set &writefds) {
    (void) writefds;
    if (FD_ISSET(master_socket, &readfds))  
    {  
        int addrlen = sizeof(address);
        int new_sd;
        if ((new_sd = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
            cerr <<  connect_except().what();
        // cout << "New connection , socket fd is "<< new_sd  <<" , ip is : " << inet_ntoa(address.sin_addr)<< ", port : "<< ntohs(address.sin_port) << endl;
        if (new_sd > 0) {
            c_sd.push_back(new_sd);
            fcntl(new_sd, F_SETFL, O_NONBLOCK);
        }
    }
}
    
int Socket::messages(fd_set &readfds, WebServ *w) {
    for (IT it = c_sd.begin(); it != c_sd.end(); it++) {
        int r;
        if (FD_ISSET(*it , &readfds))  {  
            if ((r = recv( *it , buffer, BUFF_SIZE - 1, 0)) <= 0)  {  
                //Somebody disconnected , get his details and print 
                if (r < 0)
                    cerr << ("recv failed for socket :" + to_string(*it)) << endl;
                int addrlen = sizeof(address);
                getpeername(*it , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                // cout << "Host disconneasdcted , ip " <<  inet_ntoa(address.sin_addr) << " , port " << ntohs(address.sin_port) << " sd "<< *it << endl;  
                if (*it > 0) 
                    close(*it);
                for (size_t i = 0; i < w->requests.size(); i++) {
                    if (w->requests[i].sd == *it) 
                        {w->requests.erase(w->requests.begin() + i); i--;}}
                c_sd.erase(it);it--;
                
            } else {
                int flag = 0;
                buffer[r] = 0;
                // ofstream o; o.open(("www/logs/" + to_string(*it) + "_"+ to_string(time(NULL)) + to_string(rand()%10)).data());o << buffer; o.close();
                for (size_t i = 0; i < w->requests.size(); i++) {
                    if (*it == w->requests[i].sd) {
                        flag = 1;
                        w->requests[i].body.write(buffer, r);
                        string b = w->requests[i].bound;
                        if (w->requests[i].body.str().find(w->requests[i].bound) == string::npos) {
                            w->requests.erase(w->requests.begin() + i);i--;
                            cout << "request thrown" << endl;
                            break;
                        }
                        // ------WebKitFormBoundaryAEzXNiTE5CMKVPNI--
                        b.insert(w->requests[i].bound.size(), "--");
                        b.insert(0, "--");//need to be in that order??
                        cout << b << endl;
                        if (w->requests[i].body.str().find(b) != string::npos) { //end 
                            cout << "req finished" << endl;
                            w->responses.push_back(Response(w->requests[i]));
                            w->requests.erase(w->requests.begin() + i);i--;
                        
                        }
                    }
                }
                if (flag) continue;
                Request tmp(buffer, w, *it, port);
                vector<string> q = tmp.get_val("Content-Type");
                if (!tmp.host)
					continue;
            if (q.size() && q[0] == "multipart/form-data") {
                    tmp.bound = q[1].substr(q[1].find_first_of('=') + 1);// + "\r\n";
                    cout << tmp.header <<endl;
                    w->requests.push_back(tmp);
                } else {
                    w->responses.push_back(Response(tmp));
                }
            }
        }  
    }
    return 0;  
}

 

