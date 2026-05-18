#include "game.h"

#include <bit>
#include <cstdlib>

namespace
{
    std::vector<bool> mem;
    constexpr int modulo = 981;
    constexpr int width = std::bit_width((unsigned)(modulo - 1));

    std::vector<bool> encode(int x)
    {
        std::vector<bool> c(width);
        for (int i = 0; i < width; i++)
            if (x >> i & 1)
                c[i] = true;
        return c;
    }
    int decode(std::vector<bool> c)
    {
        int x = 0;
        for (int i = 0; i < width; i++)
            if (c[i])
                x |= (1 << i);
        return x;
    }
}

int start_game(int L, int N, int i)
{
    return width;
}

std::vector<bool> leader(std::vector<bool> c)
{
    return encode((decode(c) + 1) % modulo);
}

std::vector<bool> student_first(std::vector<bool> c)
{
    mem = c;
    return c;
}

std::pair<bool, std::vector<bool>> student_second(std::vector<bool> c)
{
    return std::make_pair(mem != c ? true : (rand() % 2 == 0), c);
}