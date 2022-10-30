#pragma once
#include <sys/stat.h>
#include "webserv.hpp"
using namespace std;


template <typename T>
std::string to_string ( T Number )
{
     std::ostringstream ss;
     ss << Number;
     return ss.str();
}
vector<string> split_str(std::string s, std::string delimiter);
vector<string> split_set(std::string s, std::string delimiter);
vector<string> parse_config(string conf_path);
vector<vector<string> > parse_scope(string &scope, WebServ &);
vector<vector<string> > parse_routes(string &scope, WebServ &);
vector<string> get_all_scopes(string file, string directive, string open, string close);
bool dir_exist(const char *s);
bool replace(std::string& str, const std::string& from, const std::string& to);