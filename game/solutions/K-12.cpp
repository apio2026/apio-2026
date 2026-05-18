#include "game.h"

namespace
{
    std::vector<bool> mem;
}

int start_game(int L, int N, int i)
{
    return 12;
}

std::vector<bool> leader(std::vector<bool> c)
{
    int i = 0;
    while (i < 12 && c[i])
        c[i++] = false;
    if (i < 12)
        c[i] = true;
        
    return c;
}

std::vector<bool> student_first(std::vector<bool> c)
{
    mem = c;
    return c;
}

std::pair<bool, std::vector<bool>> student_second(std::vector<bool> c)
{
    return std::make_pair(mem != c, c);
}