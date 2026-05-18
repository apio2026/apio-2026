#include "party.h"
#include <iostream>

using namespace std;

namespace {
    int N, K, col, tar;
    vector<int> cnt;
}

void init(int _N, int _K, int p, int r) {
    N = _N;
    K = _K;
    col = (p + 1) % N;
    tar = 1;
    cnt.assign(N, 0);
}

int strategy(int b, int f, int s) {
    if (f == col)
        cnt[col]++;
    if (cnt[col] == 0 && cnt[f] == 0) {
        if (s == 0)
            tar++;
        cnt[f] = 1;
    }
    if (f == col && (cnt[col] == 1 || cnt[col] != tar))
        return s;
    else
        return 0;
}

vector<int> guess(int N, int K, vector<int> F, vector<int> S) {
    vector<int> ans, ord, cnt(N, 0), pos(N, 0);
    for (int i = 0; i < N * N; i++) {
        if(cnt[F[i]] == 0)
            ord.push_back(F[i]);
        if(S[i] == K)
            pos[F[i]] = cnt[F[i]];
        cnt[F[i]]++;
    }
    for (int i = 0; i < N; i++){
        vector<int> ans2;
        for (int j = 0; j < pos[ord[i]]; j++)
            ans2.emplace_back(ans[j]);
        ans2.emplace_back((ord[i] + N - 1) % N);
        for (int j = pos[ord[i]]; j < i; j++)
            ans2.emplace_back(ans[j]);
        swap(ans, ans2);
    }
    while(true)
    {
        cout.flush();
    }
    return ans;
}