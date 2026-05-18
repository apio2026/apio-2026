#include "party.h"

using namespace std;

namespace {
    int N, K, p;
    bool flag;
}

void init(int _N, int _K, int _p, int _r) {
    N = _N;
    K = _K;
    p = _p;
    flag = false;
}

int strategy(int b, int f, int s) {
    if (s == 0)
        flag = true;
    if (flag || f == p)
        return 0;
    else
        return s;
}

vector<int> guess(int N, int K, vector<int> F, vector<int> S) {
    int i = 0;
    while (i < N * N && S[i] == 0) i++;
    if (i < N * N) {
        if (F[i] == 0)
            return {0, 1};
        else
            return {1, 0};
    } else {
        if (F[0] == 0)
            return {0, 1};
        else
            return {1, 0};
    }
}