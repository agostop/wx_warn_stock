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
    string the_message;
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

    map<string,double> warn_val;

    getSinaJS(userstockcode,s_map);

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
        getSinaJS(userstockcode,s_map);

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


            /*
           the_message.append("test");
           if ( sendtoWX(the_message,"sh600795",userhash)!=0)
           {
               cout << "send wx message failed" << endl;
               exit(-1);
           }
           */
            if ( ( jitter - warn_line) > 0.20 )
            {

                the_message.append("注意! \"");
                the_message.append(sbg->second[0]);
                the_message.append("\"已经涨了");
                the_message.append(ConvertToString(jitter));
                the_message.append("毛钱，现在价格是\"");
                the_message.append(sbg->second[3]);
                the_message.append("\n");
               // if (sendtoWX(the_message,sbg->first,userhash))
                if (sendtoWX(the_message,sbg->first,userhash)!=0)
                    cerr << "send message failed !" << endl;

                warn_line=jitter;
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

                if (sendtoWX(the_message,sbg->first,userhash)!=0)
                    cerr << "send message failed !" << endl;

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
