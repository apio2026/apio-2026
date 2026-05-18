#include "game.h"

namespace
{
    int id;
}

int start_game(int L, int N, int i)
{
    id = i - 1;
    return N;
}

std::vector<bool> leader(std::vector<bool> c)
{
    return std::vector<bool>(c.size(), false);
}

std::vector<bool> student_first(std::vector<bool> c)
{
    c[id] = true;
    return c;
}

std::pair<bool, std::vector<bool>> student_second(std::vector<bool> c)
{
    return std::make_pair(c[id] == false, c);
}