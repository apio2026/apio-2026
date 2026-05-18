#include "ancient.h"

#include <bits/stdc++.h>
using namespace std;

pair<vector<int>, vector<int>> find_information(int N, int C)
{
    vector<int> p(N, 0);
    int max_len = 1 << __lg(N);
    {
        vector<int> offset(N, 0);
        for (int len = max_len; len >= 1; len >>= 1)
        {
            vector<long long> t[2];
            {
                int cnt = 0;
                for (int parity : {0, 1})
                {
                    t[parity].resize(N);
                    for (int i = 0; i < N; i++)
                        if (i / len % 2 == parity)
                            t[parity][i] = (long long)(cnt++) * C;
                        else
                            t[parity][i] = (long long)N * C;
                }
            }
            vector<int> r[2];
            r[0] = detect_convenience_values(t[0]);
            r[1] = detect_convenience_values(t[1]);

            // p[i]: parent i is in [p[i], p[i] + 2 * len)
            for (int i = 0; i < N; i++)
                if (r[i / len % 2][i] == i / len % 2)
                    p[i] += len;
        }
    }
    vector<int> d_lower(N, -C), d_upper(N, C);

    while (true)
    {
        bool done = true;
        for (int i = 1; i < N; i++)
            if (d_lower[i] != d_upper[i])
                done = false;
        if (done)
            break;

        vector<long long> query(N, 0);
        for (int i = 1; i < N; i++)
        {
            int d_mid = d_lower[i] + d_upper[i];
            d_mid = (d_mid < 0 ? (d_mid - 1) : d_mid) / 2;

            query[i] = query[p[i]] + d_mid;
        }

        auto r = detect_convenience_values(query);
        for (int i = N - 1; i > 0; i--)
        {
            // this >= parent
            r[p[i]] -= !r[i];
            int d_mid = d_lower[i] + d_upper[i];
            d_mid = (d_mid < 0 ? (d_mid - 1) : d_mid) / 2;
            if (r[i])
                d_upper[i] = d_mid;
            else 
                d_lower[i] = d_mid + 1;
                
        }
    }

    d_lower[0] = 0;
    for (int i = 1; i < N; i++)
        d_lower[i] += d_lower[p[i]];
    for (int i = 1; i < N; i++)
        d_lower[i] = -d_lower[i];
    p.erase(p.begin());
    d_lower.erase(d_lower.begin());
    return make_pair(p, d_lower);
}
