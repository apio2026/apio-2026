#include "nightmarket.h"
#include <valarray>
#include <map>

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    std::map<std::vector<int>, std::string> m;
    for (int msk = 0; msk < (1 << (N + M - 2)); msk++)
    {
        std::string s = "";
        std::vector<int> path = {S[0][0]};
        int x = 0;
        int y = 0;
        for (int i = 0; i < N + M - 2; i++)
        {
            if(msk >> i & 1)
            {
                s += "S";
                x++;
            }
            else
            {
                s += "E";
                y++;
            }
            if (x >= N || y >= M) break;
            path.push_back(S[x][y]);
        }
        if (x != N - 1 || y != M - 1) continue;
        if (m.count(path))
        {
            return {m[path], s};
        }
        m[path] = s;
    }
    return {};
}
