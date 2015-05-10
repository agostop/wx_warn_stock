#include "gupiao.h"
using namespace std;

string GB2312ToUTF8(const char* pStrGB2312)
{
     string s_pstr;
     // 先转成宽字符
     int nStrLen = MultiByteToWideChar(CP_ACP, 0, pStrGB2312, -1, NULL, 0);
     wchar_t* pWStr = new wchar_t[nStrLen + 1];
     memset(pWStr, 0, nStrLen + 1);
     MultiByteToWideChar(CP_ACP, 0, pStrGB2312, -1, pWStr, nStrLen);

     // 再转为UTF8
     nStrLen = WideCharToMultiByte(CP_UTF8, 0, pWStr, -1, NULL, 0, NULL, NULL);
     char* pStr = new char[nStrLen + 1];
     memset(pStr, 0, nStrLen + 1);
     WideCharToMultiByte(CP_UTF8, 0, pWStr, -1, pStr, nStrLen, NULL, NULL);
     if(pWStr)
     {
          delete[] pWStr;
     }
     s_pstr.assign(pStr);
     return s_pstr;
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, string *userp)
{
    //strcat(userp,contents);
    userp->append(contents);
    return size*nmemb;
}

string reg(const char *restr,string *data)
{
    regex re(restr);
    smatch mat;
    string token;
    if (regex_search(*data,mat,re))
    {
        token=string(mat[1].str());
        return token;
    }else
    {
        return NULL;
    }
}

void getSinaJS(vector<string> &code ,map<string,vector<string>> &s_sum)
{
    CURL *curl;
    CURLcode res;

    struct curl_slist* m_header=NULL;
    m_header = curl_slist_append(m_header,"User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2292.0 Safari/537.36");

    string *data=new string;
    string code1;
    vector<string>::iterator it_code = code.begin();
    for (;it_code!=code.end();++it_code)
    {
        code1.append(*it_code+",");
    }

    string sinaurl= "http://hq.sinajs.cn/list="+code1;

    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, sinaurl.c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER,m_header);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);


      for (int try_time=0;
           (res = curl_easy_perform(curl))!=CURLE_OK && try_time!=5;
           ++try_time)
      {
          cerr<< "can't access the sina js , retry !" << curl_easy_strerror(res) << endl;
          Sleep(1000);
      };

      curl_easy_cleanup(curl);
      curl_slist_free_all(m_header);

      if (data->empty())
      {
          cerr << "the web page can not opened." << endl;
          exit(1);
      }
      string_Process(data,s_sum);

      delete data;
    }
}
//    cout << *data << endl;

int string_Process(string *data,map<string,vector<string>> &s_sum)
{
    //map<string,vector<string>> s_sum;
    string content;
    size_t m=0,n=0;
    string tmp;
    string key;
    vector<string> strarr;
    size_t x=0,y=0;
    string cont_cut;

    content.assign(*data);

    content=GB2312ToUTF8(content.c_str());

    while ((n=content.find_first_not_of("\n",m))!=string::npos)
    {
        if ((m=content.find_first_of("\n",n))!=string::npos)
        {
           key.clear();
           tmp.clear();
           strarr.clear();
           x=0;
           y=0;
           cont_cut.clear();
           cont_cut=content.substr(n,m-n);
           key=reg("hq_str_(.*)=\".*\";" , &cont_cut);
           tmp=reg("hq_str_.*=\"(.*)\";" , &cont_cut);
           while ((x=tmp.find_first_not_of(",",y))!=string::npos)
           {
               if ((y=tmp.find_first_of(",",x))!=string::npos)
                   strarr.push_back(tmp.substr(x,y-x));
           }
           s_sum[key]=strarr;
        }
    }
    return 0;

    /*
     * 验证部分
    map<string,vector<string>>::iterator sbg=s_sum.begin();
    for (;sbg!=s_sum.end();++sbg)
    {
        cout << "the keys is : " << sbg->first << endl;
        vector<string>::iterator it=sbg->second.begin();
        for (;it!=sbg->second.end();++it)
            cout << "the value is : " << *it << endl;
    }
    */

//    curl_global_cleanup();

}
