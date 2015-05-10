#include "getconf.h"

int Getconf(const char *filepath,strhash &userhash)
{
    std::ifstream file;
	std::string confstr;
	strarray userarray;

	file.open(filepath);

	while (getline(file,confstr))
	{
        Spilt_str(confstr,", ",userarray);
		strarray tmp(++userarray.begin(),userarray.end());
		//std::cout << UTF8ToGB2312(userarray[0].c_str()) << std::endl;
		userhash[userarray[0]]=tmp;
		userarray.clear();
	}
    file.close();

    /*
	ithash it=userhash.begin();
	for (;it!=userhash.end();++it)
	{
		std::string *tmp=(it->second).data();
		std::cout << UTF8ToGB2312((it->first).c_str()) << ":\n";
		for (size_t i=0;i!=(it->second).size();++i,++tmp)
			std::cout << "\t" << *tmp << "\n";
		//std::cout << it->second << std::endl;
	}
    */

	return 0;
}


int Spilt_str (std::string str,const char *splitchar,strarray &result)
{
	std::string::size_type bg=0,ed=0;

	while((bg=str.find_first_not_of(splitchar,ed))!=std::string::npos)
	{
	    ed=str.find_first_of(splitchar,bg);
	    if (ed==std::string::npos)
	    {
	        ed=str.length();
	    }
			std::string tmp(str,bg,ed-bg);
			
	    if (tmp.size()!=0)
	        result.push_back(tmp);
	}
    return 0;

}

/*
char* UTF8ToGB2312(const char* pStrUTF8)
{
     // 先转成宽字符
     int nStrLen = MultiByteToWideChar(CP_UTF8, 0, pStrUTF8, -1, NULL, 0);
     wchar_t* pWStr = new wchar_t[nStrLen + 1];
     memset(pWStr, 0, nStrLen + 1);
     MultiByteToWideChar(CP_UTF8, 0, pStrUTF8, -1, pWStr, nStrLen);

     // 再转成GB2312
     nStrLen = WideCharToMultiByte(CP_ACP, 0, pWStr, -1, NULL, 0, NULL, NULL);
     char* pStr = new char[nStrLen + 1];
     memset(pStr, 0, nStrLen + 1);
     WideCharToMultiByte(CP_ACP, 0, pWStr, -1, pStr, nStrLen, NULL, NULL);
     if(pWStr)
     {
          delete[] pWStr;
     }
     return pStr;
}
*/
