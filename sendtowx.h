#ifndef SENDTOWX
#define SENDTOWX

#include <fstream>
#include <cstdlib>
#include <Windows.h>
#include <sstream>
#include "gupiao.h"
#include <document.h>
using namespace std;

int sendtoWX(string &rawcontent, string stockcode, map<string, vector<string> > &userhash);
string Get_fakeid(map<string,string> &userlist,string &stockcode,map<string,vector<string>> &userhash);
int Get_userlist(string *Data , map<string,string> &userlist);
template <class T> string ConvertToString(T value) ;
template <class T>	void convertFromString(T &value, const string &s);


#endif // SENDTOWX

