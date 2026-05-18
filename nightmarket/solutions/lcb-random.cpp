#include "nightmarket.h"

#include <random>
#include <algorithm>
#include <map>

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    std::mt19937 rd(8);
    std::string path = std::string(N - 1, 'S') + std::string(M - 1, 'E');
    std::map<std::vector<int>, std::string> mp;

    int T = 10000;
    while (T--)
    {
        std::shuffle(path.begin(), path.end(), rd);
        std::vector<int> nums;
        for (int i = 0, j = 0; auto c : path)
        {
            (c == 'S' ? i : j)++;
            nums.emplace_back(S[i][j]);
        }
        if (mp[nums].empty())
            mp[nums] = path;
        if (mp[nums] != path)
            return {path, mp[nums]};
    }
    return {};
}
