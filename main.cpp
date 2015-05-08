#include "gupiao.h"
#include <fstream>
#include <cstdlib>
#include <Windows.h>
#include <sstream>

using namespace std;

int sendtoWX(string &rawcontent)
{
  CURL *curl;
  CURLcode res;

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

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "username=XXX&pwd=e8esdf643c9b5ffddca3d744edf859f5&imgcode=&f=json");
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
    string token=reg(".*=(\\d+).*",data);

    /*组合获取用户列表的url*/
    string url="https://mp.weixin.qq.com/cgi-bin/contactmanage?t=user/index&pagesize=10&pageidx=0&type=0&token="+token+"&lang=zh_CN";
    string referer_page= "https://mp.weixin.qq.com/cgi-bin/home?t=home/index&lang=zh_CN&token="+token;

    curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
    curl_easy_setopt(curl, CURLOPT_REFERER,referer_page.c_str());
    curl_easy_setopt(curl, CURLOPT_POST,0);
   //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
    //如果不是post数据，就不要设置该项，不然即使设置CURLOPT_POST为0，也会将本次操作当作post
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
    {
      cout << "curl_easy_perform() failed: " << curl_easy_strerror(res)<< endl;
      return -1;
    }

    string fake_id;
    fake_id=reg("friendsList.*?contacts\":\\[\\{\"id\":(\\d+),.*",data);
    //cout << fake_id << endl;

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

template <class T>	void convertFromString(T &value, const string &s)
{
    stringstream ss(s);
    ss >> value;
}

template <class T> std::string ConvertToString(T value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

int main(int argc,char *argv[])
{
    //vector<string> s_code;
    map<string,vector<string>> s_map;
    string config;
    string the_message;
    string opt;

    ifstream conf;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    /*
    if (argc != 2)
    {
        cerr << "must have a conf file" << endl;
       // exit(1);
    }
    config.assign(argv[1]);
    */

    //conf.open(config.c_str());
    conf.open("d:/tess.txt");
    if (!conf.is_open())
    {
        cerr << "the file open failed !" << endl;
        exit(1);
    }

    vector<string> _config;
    while (conf >> opt)
        _config.push_back(opt);

    conf.close();

    map<string,double> warn_val;

    getSinaJS(_config,s_map);

    map<string,vector<string>>::iterator sbg=s_map.begin();

    for (;sbg!=s_map.end();++sbg)
        warn_val.insert(make_pair(sbg->first,0.00));
        //cout << sbg->first << endl;

   double w_val=0.00;
   double curr_val=0.00;
   double jitter=0.00;
   double warn_line=0.20;
   /*
   double red_line=0.20;
   double green_line=-0.20;
   */

    while (1)
    {
        getSinaJS(_config,s_map);

        map<string,vector<string>>::iterator sbg=s_map.begin();

        for (;sbg!=s_map.end();++sbg)
        {
            //double w_val=warn_val[sbg->first];
            if (warn_val[sbg->first]==0.00 )
                convertFromString(warn_val[sbg->first],sbg->second[1]);

            w_val=warn_val[sbg->first];
            convertFromString(curr_val,sbg->second[3]);
            // cout << curr_val << "\t\t" << warn_val << endl;

            jitter = curr_val - w_val ;

            cout << "curr_val\t" << curr_val << "\nwarn_val\t" << w_val << endl;
            cout << "the warn val is : " << jitter<< endl;


            if ( ( jitter - warn_line) > 0.20 )
            {
                the_message.append("注意! \"");
                the_message.append(sbg->second[0]);
                the_message.append("\"已经涨了");
                the_message.append(ConvertToString(jitter));
                the_message.append("毛钱，现在价格是\"");
                the_message.append(sbg->second[3]);
                the_message.append("\n");

                /*
                if (sendtoWX(the_message)<0)
                    cerr << "send message failed !" << endl;
                    */

                warn_line=jitter;
         //           warn_val[sbg->first]+=jitter;

                the_message.clear();

            }else if ( ( jitter - warn_line) < -0.20)
            {
                the_message.append("注意! \"");
                the_message.append(sbg->second[0]);
                the_message.append("\"已经跌了");
                the_message.append(ConvertToString(jitter));
                the_message.append("毛钱，现在价格是\"");
                the_message.append(sbg->second[3]);
                the_message.append("\"\n");

                /*
                if (sendtoWX(the_message)<0)
                    cerr << "send message failed !" << endl;
                    */

                warn_line=jitter;
          //          warn_val[sbg->first]+=jitter;

                the_message.clear();

            }

            /*
            the_message.append(sbg->first);
            the_message.append("\n");
            the_message.append(sbg->second.begin()[2]);
            the_message.append("\n\n");
            */

        }

        Sleep(5000);
   }
   curl_global_cleanup();
}
