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
    for (IT it = c_sd.begin() ; it < c_sd.end() ; it++)  {
        if (*it > 0)  {FD_SET(*it , &readfds);  FD_SET( *it , &writefds);}
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
            throw connect_except();
        cout << "New connection , socket fd is "<< new_sd  <<" , ip is : " << inet_ntoa(address.sin_addr)<< ", port : "<< ntohs(address.sin_port) << endl;
        c_sd.push_back(new_sd);
        fcntl(new_sd, F_SETFL, O_NONBLOCK);
    }

}
    
int Socket::messages(fd_set &readfds, WebServ *w) {
    int addrlen = sizeof(address);
    for (IT it = c_sd.begin(); it != c_sd.end(); it++) {
        int r;
        if (FD_ISSET(*it , &readfds))  {  
            if ((r = recv( *it , buffer, BUFF_SIZE, 0)) <= 0)  {  
                //Somebody disconnected , get his details and print 
                if (r < 0)
                    throw invalid_argument("recv failed");
                getpeername(*it , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                cout << "Host disconneasdcted , ip " <<  inet_ntoa(address.sin_addr) << " , port " << ntohs(address.sin_port) << " sd "<< *it << endl;  
                close(*it);c_sd.erase(it--);
            }
            else {
                buffer[r] = '\0';
                Request tmp(buffer, w, *it, port);
                w->responses.push_back(Response(tmp));
            }  
        }  
    }
    return 0;  
}



