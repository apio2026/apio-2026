#include "game.h"
#include <random>
using namespace std;

namespace
{
    std::mt19937 rd;
    int pos;
}

int start_game(int L, int N, int i)
{
    rd.seed(200 + i);
    return 64;
}

std::vector<bool> leader(std::vector<bool> c)
{
    for (int i = 0; i < 64; i++)
        if (c[i])
            c[i] = false;
    return c;
}

std::vector<bool> student_first(std::vector<bool> c)
{
    pos = rd() % 64;
    int off = rd() % 64;
    for (int i = 0; i < 64; i++)
        if (c[(i + off) % 64] == false)
        {
            pos = (i + off) % 64;
            break;
        }
    c[pos] = true;

    return c;
}

std::pair<bool, std::vector<bool>> student_second(std::vector<bool> c)
{
    auto ans = std::make_pair(c[pos] == false, c);
    c[pos] = false;
    return ans;
}