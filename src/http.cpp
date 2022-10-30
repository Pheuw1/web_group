#include "http.hpp"
typedef std::map<std::string, std::vector<std::string> >::iterator M;
Response::Response(const Request &req): w(req.w),version(req.version), status(0), description(""),headers(""), body("") ,req_cp(req) {
    M m;
    int ret;
	std::cout << "~;:"  + req.url << std::endl;
    if (req.url.find("autoindex") != string::npos)
		ret = autoindex(w.root + req.url.substr(0, req.url.find_last_of("/") + 1), *this);
    else if (((m = req.host->route_methods.find(req.url))!= req.host->route_methods.end() 
            && find(m->second.begin(), m->second.end(),req.method_name) == m->second.end()))
        ret = 405;
    else if (w.Methods.find(req.method_name) == w.Methods.end())
        ret = 400;
	else
        ret = (w.Methods[req.method_name])(req_cp, *this);
    buffer << req.version << " " << ret << " " << w.HttpStatusCode[ret] << "\r\n";
    if (ret < 400) { //body gets set by method
        headers += "Content-length: " + to_string(body.str().size()) + "\n";
        if (req.url.find("cookie")) 
            headers +=  "Set-Cookie: cookie=cookie\n";
        if (req.header.find("Cookie: ")) 
            cout << "user on sd " <<req.sd << " has a cookie" << endl;
        buffer << headers << "\r\n";
        buffer << "\r\n" << body.str() << "\r\n";\
    } else {
        ifstream file_stream;
        file_stream.open((w.root + "/" + req_cp.host->error_path + to_string(ret) + ".html").data());
        if (file_stream.fail())
            file_stream.open((w.root + "/" + w.error_path + to_string(ret) + ".html").data());
        string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
        body << file;
        headers += "Content-length: " + to_string(body.str().size()) + "\n";
        buffer << headers << "\r\n";
        buffer << "\r\n" << body.str() << "\r\n";
    }

}


Request::Request(char *buffer, WebServ *web, int sd, int port): w(*web), sd(sd), obuff(buffer), port(port){
    string file = string(buffer);
    vector<string> elems(split_set(file.substr(0,file.find("\r\n")), " "));
    //if (!get_val("Content-length").empty())https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding
    if (elems.size() != 3) throw invalid_argument("invalid request");
    method_name = elems[0];url = elems[1];version = elems[2];
    header = file.substr(file.find("\r\n") + 1,file.find("\r\n\r\n"));
    body = file.substr(file.find("\r\n\r\n"));
    host = w.get_host(get_val("Host")[0], port);
    std::cout << "asd :" + url << std::endl;
	if (url.find_last_of("/") == url.size() - 1) {
        for (map<string, string>::iterator i = host->dirs.begin(); i != host->dirs.end(); i++) {
            if (i->first == url && i->second == "autoindex")
                url = i->first + i->second;
			else if (i->first == url) 
                url = w.root + "/" + i->second;
		}
		if (url == elems[1] && url == "/")
            url = w.root + "/" + w.home;
    } else if (url.find(".html") != string::npos && url.find("/HTML") == string::npos) { 
            url = w.root + "/" + "HTML" + url;
    } else { url = w.root + url;}

}

vector<string> Request::get_val(string key) {//to get host
    string content;
    size_t start = header.find(key);
    if (start != string::npos)
        start = header.find_first_of(":", start);
    if (header.find_first_not_of(": ", start) != string::npos && header.find_first_of("\n", start) != string::npos)
        content = header.substr(header.find_first_not_of(":", start)  ,header.find_first_of("\n",start) - header.find_first_not_of(": ", start));
    return split_set(content, ";");
}

Request::~Request() {;}
Response::~Response() {;}

int GET(Request &req, Response &rep) {
    if (req.url.find(req.w.root + "/cgi-bin/") != string::npos)
        return CGI(req, rep);
    ifstream file_stream;
    file_stream.open(req.url.data());
    if (file_stream.fail()) 
        return(404);
    std::string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
    rep.body << file;//check if css file and insert in header
    return (200);
}


int DELETE(Request &req, Response &rep) {
    (void) rep;
    if (remove((req.w.root + req.url).data()))
        return(404);
    return (200);
}

//either create file or append content to existing file
int POST(Request &req, Response &rep) {
    (void) rep;
    ofstream new_file;
    new_file.open((string(req.url)).data());
    if (new_file.fail())
        return 401;
    new_file << req.body;
    return 201;
}

int CGI(Request &req, Response &rep) {
    char *av[3];
    int pip[2];
    int ret,r;
    char buffer[2048];
    string cgi = "";

    if (pipe(pip) != 0)
        return 500;
    for (vector<string>::iterator it = req.host->cgi_ext.begin(); it != req.host->cgi_ext.end(); it++)
        if (req.url.find(*it) == req.url.size() - it->size())
            cgi = *it + "-cgi"; 
    if (cgi.empty())
        return 403;
    cout << cgi << endl;    
    if (access(("/" + req.w.cwd + "/" + req.url).data(), X_OK) < 0)
        return 401;
    pid_t pid = fork();
    if (pid < 0) return 502; 
    if (!pid) {
        av[0] = strdup((char *)("/" + req.w.cwd + "/" + req.w.root + "/cgi-bin/" + cgi).c_str());
        av[1] = strdup((char *)("/" + req.w.cwd + "/" + req.url).data()); 
        av[2] = NULL;
        // if (req.body.size() && req.body.find_first_not_of("     \n\r") != string::npos) 
        //     av[1] = strdup(req.body.data());
        if (!av[0] || !av[1])
            return 500;
        cerr << av[0] << "||" << av[1]  << endl;
        close(pip[0]);
        dup2(pip[1],1);
        if (execve(av[0], av, req.w.env) == -1)
            perror("exec failed");
        exit(1);
    }
    close(pip[1]);
    if (waitpid(pid, &ret, 0) == -1)
        return 500;
    rep.body << "<body>\n";
    while ((r = read(pip[0], buffer, 2048)) != 0){
        buffer[r] = 0; cgi = string(buffer); replace(cgi, "\n", "<br>");
        rep.body << cgi;}
    rep.body << "</body>";
    if (WIFEXITED(ret) & WEXITSTATUS(ret))
        return 502;
    return 200;
}

int autoindex(string url, Response &rep) {
    DIR 			* dir; 
	struct dirent *diread;
    vector<char *> files;

    if ((dir = opendir(url.data())) != NULL) {
        while ((diread = readdir(dir)) != NULL)
            files.push_back(diread->d_name);
        closedir (dir);
    } else {return 404;}
	rep.body << "<!doctype html>\n<html>\n<head>\n<title>";
	rep.body <<	url + "</title>\n</head>\n<body>\n";
	for (size_t i = 0; i < files.size(); i++) {
		rep.body << "<p> <a href=" + string(files[i]) + ">" + string(files[i]) + "</a> </p>\n";
		rep.body << "</body>\n</html>\n";
	}	
	return (200);	
}