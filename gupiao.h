#ifndef GUPIAO_H
#define GUPIAO_H
#include <iostream>
#include <string>
#include <curl.h>
#include <regex>
#include <map>

using namespace std;

string reg(const char *restr, string *data);
size_t WriteCallback(char *contents, size_t size, size_t nmemb, string *userp);
string GB2312ToUTF8(const char* pStrGB2312);
int string_Process(string *data, map<string, vector<string>> &s_sum);
void getSinaJS(vector<string> &code , map<string,vector<string>> &s_sum);

#endif // GUPIAO_H

