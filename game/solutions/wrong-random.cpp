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
    return 20;
}

std::vector<bool> leader(std::vector<bool> c)
{
    for (int i = 0; i < 20; i++)
        if (c[i])
            c[i] = false;
    return c;
}

std::vector<bool> student_first(std::vector<bool> c)
{
    pos = rd() % 20;
    int off = rd() % 20;
    for (int i = 0; i < 20; i++)
        if (c[(i + off) % 20] == false)
        {
            pos = (i + off) % 20;
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