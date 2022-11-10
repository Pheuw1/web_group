#include "parse.hpp"
typedef vector<string>::iterator IT;
typedef vector<vector<string> >::iterator IT_2;

vector<string> split_str(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }
    res.push_back (s.substr (pos_start));
    return res;
}


vector<string> split_set(std::string s, std::string delimiter) {
    vector<string> ret;
    size_t pos_start = 0, pos_end = 0;
    std::string token;

    while ((pos_start = s.find_first_not_of(delimiter, pos_end)) != string::npos) {
        pos_start = s.find_first_not_of(delimiter, pos_end);
        pos_end = s.find_first_of(delimiter, pos_start);
        ret.push_back(s.substr(pos_start, pos_end - pos_start));
    }
    return ret;
}

vector<string> get_all_scopes(string file, string directive, string open, string close) {
    size_t start, end = 0;
    string scope;
    vector<string> res;
    for (size_t i = 0; (i = file.find(directive, end)) != string::npos;){
        if ((i = file.find_first_of(open, i)) == string::npos) throw invalid_argument("couldn't open scope"); 
        i = file.find_first_not_of("     \n", i); 
        if (!file[i + 1] && (i == string::npos || file[i] != open[0])) {throw invalid_argument("couldn't close scope");}
        start = i + 1;
        if ((end = file.find(close, start)) == string::npos) {throw invalid_argument("scope didn't end");}
        if ((scope = file.substr(start, end - 1 - start)).find_first_of(open + close) != string::npos) {throw invalid_argument("");}
        res.push_back(scope);
    }
    return res;
}

vector<string> parse_config(string conf_path) {
    ifstream file_stream;
    file_stream.open(conf_path.data());
    if (!file_stream)
        throw invalid_argument("config file doesnt exist\n");
    std::string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
    file_stream.close();
    size_t i;
    while ((i = file.find("#")) != string::npos)
            file.erase(i, file.find_first_of("\n", i) - i);
    cout << file << endl;
    return get_all_scopes(file, "server", "{", "}");
}

vector<vector<string> > parse_routes(string &scope, WebServ &w) {
    (void)w;
    vector<vector<string> > ret;
    vector<string> tmp;
    if (!scope.size())
        return ret;
    vector<string> lines = split_set(scope, "\n");
    for (vector<string>::iterator it = lines.begin(); it != lines.end(); it++)
        if (it->size() > 0 && it->find_first_not_of("    ,:") != string::npos)
            if ((tmp = split_set(*it, "	 :,")).size() >= 1)// >= 1
                ret.push_back(tmp);
    return ret;
}


//this for each server scope that is found
//may need to limit cgi exts

vector<vector<string> > parse_scope(string &scope, WebServ &w) {
    vector<vector<string> > res;
    string params[] = {"listen" ,"server_name" ,"errors" ,"max_body_size", "cgi", "root", "dir"};
    vector<string> lines = split_set(scope, "\n");
    for (IT it = lines.begin(); it < lines.end(); it++)
        if ((*it).size() > 0 && (*it).find_first_not_of("  ") != string::npos)
            res.push_back(split_set(*it, "  ="));
    int is;
    for (IT_2 it2 = res.begin(); it2 < res.end(); it2++) {
        is = 0;
        for (int i = 0; i < 7; i++)
            if ((*it2).size())
                is += (params[i] == (*it2)[0]);
        if (!is)//check if valid name
            throw invalid_argument("wrong argument: |" + (*it2)[0] + "|");//
        if ((*it2)[0] == "errors" && (it2->size() != 2 || !dir_exist((w.root+ "/"+ (*it2)[1]).data()))) //no more than one arg & check if exists
            throw invalid_argument("errors : invalid value");
        if ((*it2)[0] == "cgi") {
            for (IT i = it2->begin() + 1; i != it2->end(); i++){
                if (i->find_first_of(".") != string::npos)
                    throw invalid_argument("cgi : remove '.' from extension");
                if (w.cgi_exts.find(*i) == w.cgi_exts.end())            
                    throw invalid_argument("cgi : invalid extension " + *i);}
        }
        else if ((*it2)[0] == "max_body_size" && (distance((*it2).begin(), (*it2).end()) != 2) 
                && (atoi((*it2)[0].data()) < 0 || (atoi((*it2)[0].data()) == 0 && (*it2)[1] != "0"))) // check if positive
            throw invalid_argument("max_body_size : invalid value");
    }
    return res;
}


bool dir_exist(const char *s) {
    struct stat info;
    if(stat( s, &info ) != 0)
        return 0;
    else if(info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void clean_dup(string &str, char c) {
    size_t len = 0;
    for (size_t i = 0; i != string::npos;i++) {
        i = str.find_first_of(c, i);
        if (i == string::npos)
            break;
        len = 0;
        while (str[i + len] == c)
            len++;
        if (len > 1)
            str.erase(i + 1, len - 1);
    }
}
