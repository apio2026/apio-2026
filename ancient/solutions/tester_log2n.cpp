#include "ancient.h"

#include <bits/stdc++.h>
using namespace std;

pair<vector<int>, vector<int>> find_information(int N, int C)
{
    vector<int> p(N, -1), d(N);
    // returns #(child tower smaller) + [parent tower smaller or equal]
    // guess 2 2 2 2 2 2 2 1 1 1 1 1 1 1 => gives how many pairs across this place?
    //       (count if parent is in same section)
    //       (count child if in later section)
    //                     (never count child)
    //       10 10 10 10 10         9 8 7 6 5 4 3
    //       (always count parent)  (always not count)
    //       (count child if in later section)
    //                              (always count child)

    int max_len = 1 << __lg(N);

    for (int len = max_len; len >= 1; len >>= 1)
    {
        vector<long long> t(N);
        long long offset = (long long)N * C;
        for (int i = 0; i < N; i++)
            t[i] = (long long)(N - ((i / len * len) + (len - i % len))) * C;

        auto r = detect_convenience_values(t);
        for (int i = 0; i < N; i++)
            if (i / len % 2 == 0 && p[i] == -1 && i != 0)
                r[i]--;
        for (int i = 0; i < N; i++)
            if (p[i] != -1)
            {
                    r[p[i]]--;
            }

        vector<int> parents;
        vector<int> children;
        vector<int> child_l, child_r;
        for (int i = 0; i < N; i++)
            if (i / len % 2 == 0 && r[i])
                parents.emplace_back(i);
            else if (i / len % 2 == 1 && r[i] == 0 && p[i] == -1)
            {
                children.emplace_back(i);
                child_l.emplace_back(0);
                child_r.emplace_back((int)parents.size() - 1);
            }


        while (true)
        {
            bool done = true;
            for (int i = 0; i < children.size(); i++)
                if (child_l[i] != child_r[i])
                    done = false;
            if (done)
                break;

            vector<long long> query(N, 2LL * N * C);
            for (int i = 0; i < (int)parents.size(); i++)
                query[parents[i]] = 2LL * i * C;
            for (int i = 0; i < (int)children.size(); i++)
            {
                int mid = (child_l[i] + child_r[i]) / 2;
                query[children[i]] = (2LL * mid + 1) * C;
            }
            auto res = detect_convenience_values(query);
            for (int i = 0; i < (int)children.size(); i++)
            {
                int mid = (child_l[i] + child_r[i]) / 2;
                if (res[children[i]] == 1)
                    child_r[i] = mid;
                else
                    child_l[i] = mid + 1;
            }
        }
        for (int i = 0; i < (int)children.size(); i++)
            p[children[i]] = parents[child_l[i]];
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
    d.erase(d.begin());
    return make_pair(vector<int>(p.begin() + 1, p.end()), d);
}
