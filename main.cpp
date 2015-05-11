#include "gupiao.h"
#include "getconf.h"
#include "sendtowx.h"

using namespace std;

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
    map<string,vector<string>> userhash;
    map<string,vector<string>> s_map;
    string config;
   // string opt;

    //ifstream conf;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (argc != 2)
    {
        cerr << "must have a conf file" << endl;
        exit(1);
    }
    config.assign(argv[1]);

    //conf.open(config.c_str());
   // conf.open("d:/tess.txt");
    Getconf(config.c_str(),userhash);
    /*
    if (!conf.is_open())
    {
        cerr << "the file open failed !" << endl;
        exit(1);
    }
    */

    map<string,vector<string>>::iterator itmap=userhash.begin();
    vector<string> userstockcode;
    for (;itmap!=userhash.end();++itmap)
    {
        vector<string>::iterator i = itmap->second.begin();
        for (;i!=itmap->second.end();++i)
        {
           // cout << *i << endl;
            userstockcode.push_back(*i);
        }
    }
    /*
    vector<string>::iterator vit=userstockcode.begin();
    for(;vit!=userstockcode.end();++vit)
        cout << *vit << endl;


    /*
    vector<string> _config;
    while (conf >> opt)
        _config.push_back(opt);

    conf.close();
    */

    map<string,double> today_opening_val; //目前股票的开盘价格
    map<string,double> code_warn_val;   //目前股票的警告线
    map<string,double> code_jitter_val;  //目前股票的波动值

    getSinaJS(userstockcode,s_map);

    map<string,vector<string>>::iterator sbg=s_map.begin();

    for (;sbg!=s_map.end();++sbg)
    {
        today_opening_val.insert(make_pair(sbg->first,0.00));
        code_warn_val.insert(make_pair(sbg->first,0.00));
        code_jitter_val.insert(make_pair(sbg->first,0.00));
    }
        //cout << sbg->first << endl;

   double open_val=0.00;
   double curr_val=0.00;
   //double jitter=0.00;
   //double warn_line=0.00;
   /*
   double red_line=0.20;
   double green_line=-0.20;
   */
    string codename;
    string the_message;

    while (1)
    {
        getSinaJS(userstockcode,s_map);

        map<string,vector<string>>::iterator sbg=s_map.begin();

        for (;sbg!=s_map.end();++sbg)
        {
            codename=sbg->first;
            //if (today_opening_val[codename]==0.00 )  //初始化目前股票开盘价
            convertFromString(today_opening_val[codename],sbg->second[1]);
            convertFromString(curr_val,sbg->second[3]); //获取目前股票的当前价格

            open_val=today_opening_val[codename];
            // cout << curr_val << "\t\t" << warn_val << endl;

            code_jitter_val[codename] = curr_val - open_val ; //目前股票价格的波动值

            cout << "curr_val\t" << curr_val << "\nwarn_val\t" << open_val << endl;
            cout << "the jitter_val is : " << code_jitter_val[codename] << endl;
            /*
           the_message.append("test");
           if ( sendtoWX(the_message,"sh600795",userhash)!=0)
           {
               cout << "send wx message failed" << endl;
               exit(-1);
           }
           */
            if ( ( code_jitter_val[codename] - code_warn_val[codename]) > 0.20 ) // 涨价幅度大于0.20
            {

                the_message.append(GB2312ToUTF8("注意! \""));
                the_message.append(sbg->second[0]);
                the_message.append(GB2312ToUTF8("\"已经涨价了"));
                the_message.append(ConvertToString(code_jitter_val[codename]));
                the_message.append(GB2312ToUTF8("现在价格是：\""));
                the_message.append(sbg->second[3]);
                the_message.append("\"");

                if (sendtoWX(the_message,codename,userhash)!=0)
                    cerr << "send message failed !" << endl;

                code_warn_val[codename]=code_jitter_val[codename];
                the_message.clear();

            }else if ( ( code_jitter_val[codename] - code_warn_val[codename]) < -0.20 ) // 降价幅度大于-0.20
            {
                the_message.append(GB2312ToUTF8("注意! \""));
                the_message.append(sbg->second[0]);
                the_message.append(GB2312ToUTF8("\"已经涨价了"));
                the_message.append(ConvertToString(code_jitter_val[codename]));
                the_message.append(GB2312ToUTF8("现在价格是：\""));
                the_message.append(sbg->second[3]);
                the_message.append("\"");

                if (sendtoWX(the_message,codename,userhash)!=0)
                    cerr << "send message failed !" << endl;

                code_warn_val[codename]=code_jitter_val[codename];
                the_message.clear();

            }
            Sleep(5000);


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
