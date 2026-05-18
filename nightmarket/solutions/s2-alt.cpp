#include "nightmarket.h"
#include <valarray>
#include <map>
#include <utility>

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    std::map<std::vector<int>, std::string> m;
    auto gen = [&](int p) -> std::pair<std::string, std::vector<int>>
    {
        std::string s(M, 'E');
        s[p] = 'S';
        std::vector<int> path;
        for (int i = 0; i <= p; i++) path.push_back(S[0][i]);
        for (int i = p; i < M; i++) path.push_back(S[1][i]);
        return {s, path};
    };
    for(int i = 0; i < M; i++)
    {
        auto [si, pathi] = gen(i);
        for(int j = 0; j < i; j++)
        {
            auto [sj, pathj] = gen(j);
            if(pathi == pathj)
            {
                return {si, sj};
            }
        }
    }
    return {};
}
