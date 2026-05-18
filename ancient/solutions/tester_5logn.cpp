#include "ancient.h"

#include <bits/stdc++.h>
using namespace std;

pair<vector<int>, vector<int>> find_information(int N, int C)
{
    vector<int> p(N, 0), d(N);
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
    vector<int> dep(N);
    for (int i = 1; i < N; i++)
        dep[i] = dep[p[i]] + 1;

    for (int dep_mod = 0; dep_mod < 3; dep_mod++)
    {
        vector<int> parents;
        vector<int> children;
        for (int i = 0; i < N; i++)
            if (dep[i] % 3 == dep_mod)
                parents.emplace_back(i);
        for (int i = 0; i < N; i++)
            if (dep[i] % 3 == (dep_mod + 1) % 3 && i != 0)
                children.emplace_back(i);
        vector<int> child_l(children.size(), -C), child_r(children.size(), C);
        while (true)
        {
            bool done = true;
            for (int i = 0; i < children.size(); i++)
                if (child_l[i] != child_r[i])
                    done = false;
            if (done)
                break;

            vector<long long> query(N, 2 * C);
            for (int i = 0; i < (int)parents.size(); i++)
                query[parents[i]] = 0;
            for (int i = 0; i < (int)children.size(); i++)
            {
                int mid = child_l[i] + child_r[i];
                mid = (mid < 0 ? (mid - 1) / 2 : mid / 2);
                query[children[i]] = mid;
            }
            auto res = detect_convenience_values(query);
            for (int i = 0; i < (int)children.size(); i++)
            {
                int mid = child_l[i] + child_r[i];
                mid = (mid < 0 ? (mid - 1) / 2 : mid / 2);
                if (res[children[i]] == 1)
                    child_r[i] = mid;
                else
                    child_l[i] = mid + 1;
            }
        }
        for (int i = 0; i < (int)children.size(); i++)
            d[children[i]] = -child_l[i];
    }
    for (int i = 1; i < N; i++)
        d[i] += d[p[i]];
    p.erase(p.begin());
    d.erase(d.begin());
    return make_pair(p, d);
}
