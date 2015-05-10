#ifndef GETCONF_H
#define GETCONF_H
//#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <Windows.h>
#include <map>

typedef std::vector<std::string> strarray;
typedef std::map<std::string,strarray> strhash;
typedef std::map<std::string,strarray>::iterator ithash;

//char* UTF8ToGB2312(const char* pStrUTF8);
int Getconf(const char *filepath,strhash &userhash);
int Spilt_str (std::string str,const char *splitchar,strarray &result);

#endif
