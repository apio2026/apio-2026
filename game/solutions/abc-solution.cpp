#include "game.h"

namespace {
int num;
} // namespace

int start_game(int L, int N, int i) {
    return 8;
}

std::vector<bool> leader(std::vector<bool> c) {
    if (c.back() == 1) {
        c.back() = 0;
        for (int i = 6; ~i; --i) {
            c[i] = !c[i];
            if (c[i]) break;
        }
    }
    return c;
}

std::vector<bool> student_first(std::vector<bool> c) {
    for (int i = 0; i < 7; ++i) num = num * 2 + c[i];
    c.back() = 1;
    return c;
}

std::pair<bool, std::vector<bool>> student_second(std::vector<bool> c) {
    int now = 0;
    for (int i = 0; i < 7; ++i) now = now * 2 + c[i];
    return {num != now, c};
}