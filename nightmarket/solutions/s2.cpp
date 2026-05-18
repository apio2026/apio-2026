#include "nightmarket.h"
#include <valarray>
#include <map>

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    std::map<std::vector<int>, std::string> m;
    for (int p = 0; p < M; p++)
    {
        std::string s(M, 'E');
        s[p] = 'S';
        std::vector<int> path;
        for (int i = 0; i <= p; i++) path.push_back(S[0][i]);
        for (int i = p; i < M; i++) path.push_back(S[1][i]);
        if (m.count(path))
        {
            return {m[path], s};
        }
        m[path] = s;
    }
    return {};
}
