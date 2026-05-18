#include "game.h"

namespace
{
    std::vector<bool> mem;
}

int start_game(int L, int N, int i)
{
    return 8;
}

std::vector<bool> leader(std::vector<bool> c)
{
    if (c[0])
    {
        int i = 1;
        while (i < 8 && c[i])
            c[i++] = false;
        if (i < 8)
            c[i] = true;
        c[0] = false;
    }
    c.emplace_back(false);
    return c;
}

std::vector<bool> student_first(std::vector<bool> c)
{
    c[0] = true;
    mem = c;
    return c;
}

std::pair<bool, std::vector<bool>> student_second(std::vector<bool> c)
{
    return std::make_pair(mem != c, c);
}