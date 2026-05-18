#include "game.h"
#include <cstdlib>
#include <chrono>

namespace
{
    std::vector<bool> mem;
}

int start_game(int L, int N, int i)
{
    srand(std::chrono::steady_clock::now().time_since_epoch().count());
    return 1;
}

std::vector<bool> leader(std::vector<bool> c)
{
    return {rand() % 2 == 0};
}

std::vector<bool> student_first(std::vector<bool> c)
{
    return {rand() % 2 == 0};
}

std::pair<bool, std::vector<bool>> student_second(std::vector<bool> c)
{
    return {rand() % 2 == 0, {rand() % 2 == 0}};
}