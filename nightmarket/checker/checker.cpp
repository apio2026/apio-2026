#include "testlib.h"

#include <string>
#include <vector>

const std::string input_secret = "ME7ejctSVkoKUc57oSEJdTsVO92dpT3k";
const std::string output_secret = "oOHMFNtb7jCK0WnXKrePqIIUlLLAkfMo";

int main(int argc, char *argv[])
{
    registerChecker("nightmarket", argc, argv);
    readBothSecrets(output_secret);
    readBothGraderResults();
    inf.readSecret(input_secret);

    int n=inf.readInt();
    int m=inf.readInt();
    std::vector<std::vector<int>> a(n,std::vector<int>(m));
    for(int i=0;i<n;i++) for(int j=0;j<m;j++) a[i][j]=inf.readInt();

    bool partial=false;
    std::string partial_message="";
    auto checkAns=[&](InStream& stream,bool is_jury) -> bool
    {
        int k=stream.readInt();
        if(k==0) return false;
        else if(k==2)
        {
            std::vector<std::string> v=stream.readTokens(k,"[ES]*");
            for(std::string s:v)
            {
                int cntS=0;
                int cntE=0;
                for(char c:s)
                {
                    if(c=='S') cntS++;
                    else cntE++;
                }
                if(cntS!=n-1||cntE!=m-1)
                {
                    TResult res;
                    if(is_jury) res=_fail;
                    else res=_wa;
                    quitf(res,"Number of S/E in sequence is wrong");
                }
            }
            if(v[0]==v[1])
            {
                std::string msg="Returned sequences are not different";
                if(is_jury) quitf(_fail,msg.c_str());
                else
                {
                    partial=true;
                    partial_message=msg;
                    return true;
                }
            }
            std::vector<std::vector<int> > paths;
            for(std::string s:v)
            {
                std::vector<int> path={a[0][0]};
                int x=0,y=0;
                for(char c:s)
                {
                    if(c=='S') x++;
                    else y++;
                    path.push_back(a[x][y]);
                }
                paths.push_back(path);
            }
            if(paths[0]!=paths[1])
            {
                std::string msg="Returned paths don't have same cookie sequence";

                if(is_jury) quitf(_fail,msg.c_str());
                else
                {
                    partial=true;
                    partial_message=msg;
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
        else quitf(_wa,"Returned array length is neither 0 nor 2");
    };

    bool jans=checkAns(ans,true);
    bool pans=checkAns(ouf,false);
    if(jans)
    {
        if(pans)
        {
            if(partial) quitp(0.5,partial_message.c_str());
            else quitf(_ok,"Valid solution");
        }
        else quitf(_wa,"Jury found solution but participant did not");
    }
    else
    {
        if(pans)
        {
            if(partial) quitf(_wa,partial_message.c_str());
            else quitf(_fail,"Participant found solution but jury did not");
        }
        else quitf(_ok,"No solution");
    }
}
