#include "sendtowx.h"
#include "gupiao.h"
using namespace std;
using namespace rapidjson;

template <class T>	void convertFromString(T &value, const string &s)
{
    stringstream ss(s);
    ss >> value;
}

template <class T> string ConvertToString(T value) {
    stringstream ss;
    ss << value;
    return ss.str();
}

int Get_userlist(string *Data , map<string,string> &userlist)
{
    string jsonstr;
    Document d;

    jsonstr=reg("friendsList\\s:\\s[^\\[]*(\\[.*\\]).*",Data);
    d.Parse(jsonstr.c_str());

    if (d.HasParseError())
    {
        printf("GetParseError%s\n",d.GetParseError());
        return -1;
    }

    for (int i=0;i!=d.Size();++i)
    {
        userlist[d[i]["nick_name"].GetString()]=ConvertToString(d[i]["id"].GetInt());
    }

    /*
    map<string,string>::iterator it=userlist.begin();
    for (;it!=userlist.end();++it)
         cout << it->first << "\t" << it->second << endl;
         */

    return 0;

}

string Get_fakeid(
        map<string,string> &userlist,
        string &stockcode,
        map<string,vector<string>> &userhash
        )
{
   string fakeid="";
   map<string,vector<string>>::iterator userandcode=userhash.begin();
   for (;userandcode!=userhash.end();++userandcode)
   {
        vector<string>::iterator codeofone =(userandcode->second).begin();
        for (;codeofone !=(userandcode->second).end();++codeofone)
        {
            if (codeofone->compare(stockcode)==0)
            {
                fakeid=userlist[userandcode->first];
            }
        }
   }

   return fakeid;
}

int sendtoWX(string &rawcontent,string stockcode , map<string,vector<string>> &userhash)
{
  CURL *curl;
  CURLcode res;
  map<string,string> userlist;

  struct curl_slist* m_header=NULL;
  m_header = curl_slist_append(m_header,"User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2292.0 Safari/537.36");
  m_header = curl_slist_append(m_header,"Content-Type: application/json");

  string *data=new string;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://mp.weixin.qq.com/cgi-bin/login");
    curl_easy_setopt(curl, CURLOPT_REFERER, "https://mp.weixin.qq.com/");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,m_header);
    curl_easy_setopt(curl, CURLOPT_POST,1);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,"username=XXX&pwd=esdfq5643c9b5ffddca3d744edf859f5&imgcode=&f=json") ; //这里把XXX换成登录的用户名，后面的pwd跟的是密码的md5值，用户名需要转换成httpcode字符串


    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
    /*需要记录 cookie*/
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "d:/test.cookie");
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "d:/test.cookie");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
   // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
    {
      cout << "curl_easy_perform() failed" << curl_easy_strerror(res) << endl;
      return -1;
    }

    /*获取token*/
    string token(reg(".*=(\\d+).*",data));

    /*组合获取用户列表的url*/
    string url="https://mp.weixin.qq.com/cgi-bin/contactmanage?t=user/index&pagesize=10&pageidx=0&type=0&token="+token+"&lang=zh_CN";
    string referer_page= "https://mp.weixin.qq.com/cgi-bin/home?t=home/index&lang=zh_CN&token="+token;

    curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
    curl_easy_setopt(curl, CURLOPT_REFERER,referer_page.c_str());
    curl_easy_setopt(curl, CURLOPT_POST,0);
   //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
    //如果不是post数据，就不要设置该项，不然即使设置CURLOPT_POST为0，也会将本次操作当作post
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    data->clear();
    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
    {
      cout << "curl_easy_perform() failed: " << curl_easy_strerror(res)<< endl;
      return -1;
    }

    //fake_id=reg("friendsList.*?contacts\":\\[\\{\"id\":(\\d+),.*",data);
    //cout << fake_id << endl;
    Get_userlist(data,userlist);
    /*
    map<string,string>::iterator it=userlist.begin();
    for (;it!=userlist.end();++it)
    {
        cout << it->first <<"\t"<< it->second << endl;
    }
    */
    string fake_id(Get_fakeid(userlist,stockcode,userhash));
    if (fake_id.empty())
        return -1;

    url.clear();
    referer_page.clear();

    referer_page="https://mp.weixin.qq.com/cgi-bin/singlesendpage?t=message/send&action=index&tofakeid=" + fake_id + "&token=" + token + "&lang=zh_CN";
    url="https://mp.weixin.qq.com/cgi-bin/singlesend?t=ajax-response&f=json&token=" + token + "&lang=zh_CN";

    /*在这里添加要发送的内容*/
    string content(curl_easy_escape(curl, rawcontent.c_str(), rawcontent.length()));
    //cout << content << endl;

    string postval("token="+token+"&lang=zh_CN&f=json&ajax=1&random=0.8670975440181792&type=1&content="+ content + "&tofakeid="+ fake_id + "&imgcode=");

    curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
    curl_easy_setopt(curl, CURLOPT_REFERER,referer_page.c_str());
    curl_easy_setopt(curl, CURLOPT_POST,1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postval.c_str());
   // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印curl执行时的详细信息

    data->clear();
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
      cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
      return -1;
    }

    cout << *data << endl;

    curl_easy_cleanup(curl);
  }

  curl_slist_free_all(m_header);

  delete data;

  return 0;
}
