#include <algorithm>
#include <cassert>
#include <csignal>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <format>
#include <iostream>
#include <iomanip>
#include <tuple>
#include <vector>
#include <random>

using namespace std::literals::string_literals;

constexpr int NUM_PROCS = 2;

enum class verdicts
{
    judge_failure,
    protocol_violation,
    correct,
    wrong,
    partial
};

[[noreturn]] void quit(verdicts verdict, std::string reason, double score = 0.0)
{
    const char *verdict_str = nullptr;
    int exit_code = EXIT_SUCCESS;

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wswitch"
    switch (verdict)
    {
    case verdicts::judge_failure:
        verdict_str = "Judge Failure";
        exit_code = EXIT_FAILURE;
        break;
    case verdicts::protocol_violation:
        verdict_str = "Protocol Violation";
        break;
    case verdicts::correct:
        verdict_str = "translate:success";
        break;
    case verdicts::partial:
        verdict_str = "translate:partial";
        break;
    case verdicts::wrong:
        verdict_str = "translate:wrong";
        break;
    }
#pragma GCC diagnostic pop

    std::cout << std::fixed << std::setprecision(6) << score << std::endl;
    std::cerr << verdict_str << std::endl;
    std::cerr << reason << std::endl;
    std::exit(exit_code);
}
[[noreturn]] void die(verdicts verdict, const char *reason);

struct solution
{
    static constexpr int secret_g2m = 0xABC86410;
    static constexpr int secret_m2g = 0x97532FF0;
    static constexpr int code_mask = 0x0000000F;

    static constexpr int M2G_CODE_OK = 0;
    static constexpr int M2G_CODE_DIE = 1;

    static constexpr int G2M_CODE_OK = 0;
    static constexpr int G2M_CODE_WA_INVALID_LENGTH = 1;
    static constexpr int G2M_CODE_CANNOT_EXECV = 2;
    static constexpr int G2M_CODE_PV_CALL_EXIT = 13;
    static constexpr int G2M_CODE_PV_TAMPER_M2G = 14;
    static constexpr int G2M_CODE_SILENT = 15;

    FILE *mgr2sol = nullptr;
    FILE *sol2mgr = nullptr;

    void open(const char *mgr2sol_filename, const char *sol2mgr_filename)
    {
        // In this order
        mgr2sol = fopen(mgr2sol_filename, "a");
        if (!mgr2sol)
            quit(verdicts::judge_failure, "Could not open file "s + mgr2sol_filename);

        sol2mgr = fopen(sol2mgr_filename, "r");
        if (!sol2mgr)
            quit(verdicts::judge_failure, "Could not open file "s + sol2mgr_filename);
    }

    solution() {}
    solution(const char *mgr2sol_filename, const char *sol2mgr_filename)
    {
        open(mgr2sol_filename, sol2mgr_filename);
    }

    void close()
    {
        if (mgr2sol)
            fclose(mgr2sol), mgr2sol = nullptr;
        if (sol2mgr)
            fclose(sol2mgr), sol2mgr = nullptr;
    }

    void flush()
    {
        if (mgr2sol)
            fflush(mgr2sol);
    }

    void write(std::integral auto x)
    {
        if (mgr2sol == nullptr || fwrite(reinterpret_cast<char *>(&x), sizeof(x), 1, mgr2sol) != 1)
            close();
    }

    void write_secret(int m2g_code = M2G_CODE_OK)
    {
        write(secret_m2g | m2g_code);
    }

    void read_secret()
    {
        int secret = read<int>();
        if ((secret & ~code_mask) != secret_g2m)
            die(verdicts::protocol_violation, "Possible tampering with sol2mgr");
        int g2m_code = secret & code_mask;
        switch (g2m_code)
        {
        case G2M_CODE_OK:
            return;
        case G2M_CODE_WA_INVALID_LENGTH:
            die(verdicts::wrong, "Invalid length of boolean array returned");
        case G2M_CODE_CANNOT_EXECV:
            die(verdicts::protocol_violation, "Grader cannot restart itself via execv");
        case G2M_CODE_SILENT:
            quit(verdicts::judge_failure, "Unexpected g2m_code SILENT from sol2mgr");
        case G2M_CODE_PV_TAMPER_M2G:
            die(verdicts::protocol_violation, "Possible tampering with mgr2sol");
        case G2M_CODE_PV_CALL_EXIT:
            die(verdicts::protocol_violation, "Solution called exit()");
        default:
            quit(verdicts::judge_failure, "Unknown g2m_code "s + std::to_string(g2m_code) + " from sol2mgr");
        }
    }

    template <std::integral T>
    T read()
    {
        T x;
        if (sol2mgr == nullptr || fread(reinterpret_cast<char *>(&x), sizeof(x), 1, sol2mgr) != 1)
        {
            close();
            die(verdicts::protocol_violation, "manual PV, can't read int from grader");
        }
        return x;
    }
} sol[NUM_PROCS];

[[noreturn]] void die(verdicts verdict, const char *reason)
{
    for (int i = 0; i < NUM_PROCS; i++)
        sol[i].write_secret(solution::M2G_CODE_DIE);
    quit(verdict, reason);
}

void init_communication(int argc, char **argv)
{
    std::signal(SIGPIPE, SIG_IGN);
    {
        int required_args = 1 + 2 * NUM_PROCS;
        if (argc < required_args || required_args + 1 < argc)
        {
            std::string usage = argv[0];
            for (int i = 0; i < NUM_PROCS; i++)
                usage += std::format(" sol{0}-to-mgr mgr-to-sol{0}", i);

            std::cerr << "Invalid number of arguments: " << argc << std::endl
                      << "Usage: " << usage << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NUM_PROCS; i++)
        sol[i].open(argv[2 * i + 2], argv[2 * i + 1]);
}

// Judge libs

template <typename T>
concept PRNG = requires(T rng) {
    { rng() } -> std::convertible_to<unsigned int>;
};

// returns uniform number in [0, r]
// https://arxiv.org/pdf/1805.10941
unsigned int uniform_distrib(unsigned r, PRNG auto &rng)
{
    unsigned x = rng();
    auto m = (unsigned long long)x * r;
    unsigned l = m;
    if (l < r)
    {
        unsigned t = -r % r;
        while (l < t)
        {
            x = rng();
            m = (unsigned long long)x * r;
            l = m;
        }
    }
    return (unsigned)(m >> 32);
}

template <typename T>
void shuffle(std::vector<T> &arr, PRNG auto &rng)
{
    int n = (int)arr.size();
    for (int i = n - 1; i >= 1; i--)
    {
        int j = uniform_distrib(i + 1, rng);
        if (i != j)
            std::swap(arr[i], arr[j]);
    }
}

// Judge strategies

struct Game {
    int N, K;
    std::vector<int> P, F, S;
    Game() = default;
    Game(int _N, int _K, std::vector<int> _P, std::vector<int> _F) {
        N = _N;
        K = _K;
        P = _P;
        F = _F;
        S.assign(N * N, K);
    }
};

std::mt19937 rng(48763);

int main(int argc, char **argv)
{
    // Prepare solutions
    init_communication(argc, argv);

    // Communication
    auto &current = sol[0], &next = sol[1];

    int reveal_level;
    if (!(std::cin >> reveal_level))
        quit(verdicts::judge_failure, "Cannot read reveal_level from input");

    int T;
    if (!(std::cin >> T))
        quit(verdicts::judge_failure, "Cannot read T from input");

    std::vector<Game> game(T);
    std::vector<int> state(T, 0);
    std::vector<int> order;
    for (int g = 0; g < T; g++) {
        int N, K;
        if (!(std::cin >> N))
            quit(verdicts::judge_failure, "Cannot read N from input");
        if (!(std::cin >> K))
            quit(verdicts::judge_failure, "Cannot read K from input");
        
        std::vector<int> P(N), F(N * N);
        for (int i = 0; i < N ; i++) {
            if (!(std::cin >> P[i]))
                quit(verdicts::judge_failure, "Cannot read P[i] from input");
        }
        for (int i = 0; i < N * N ; i++) {
            if (!(std::cin >> F[i]))
                quit(verdicts::judge_failure, "Cannot read F[i] from input");
        }

        game[g] = Game(N, K, P, F);
        for (int i = 0; i < N + 1; i++) order.push_back(g);
    }

    shuffle(order.begin(), order.end(), rng);
    for (int g : order) {
        int i = state[g];
        state[g]++;

        int N = game[g].N;
        int K = game[g].K;
        auto& P = game[g].P;
        auto& F = game[g].F;
        auto& S = game[g].S;

        current.read_secret();
        if (i < N) { // rooms 0 ~ N - 1
            current.write_secret();
            current.write(true);
            current.write(N);
            current.write(K);
            current.write(P[i]);
            int r;
            if (reveal_level == 0)
                r = -1;
            else
                r = (i % reveal_level == 0 ? i : -1);
            current.write(r);
            current.flush();

            for(int b = 0; b < N * N; b++) {
                current.write_secret();
                current.write(F[b]);
                current.write(S[b]);
                current.flush();

                current.read_secret();
                int x = current.read<int>();

                if (P[i] == F[b] && x > 0)
                    die(verdicts::wrong, "x not zero when P[i] == F[b]");
                if (x < 0 || x > S[b])
                    die(verdicts::wrong, "x not within [0, s]");

                S[b] -= x;
            }
        } else { // guests outside
            current.write_secret();
            current.write(true);
            current.write(N);
            current.write(K);
            current.write(-1);
            current.write(N);
            current.flush();

            current.write_secret();
            for (int i = 0; i < N * N; i++)
                current.write(F[i]);
            for (int i = 0; i < N * N; i++)
                current.write(S[i]);
            current.flush();

            current.read_secret();
            int sz = current.read<int>();
            if (sz != N)
                die(verdicts::wrong, "Guessed array length is not N");
            
            std::vector<int> Q(sz);
            for (int i = 0; i < sz; i++)
                Q[i] = current.read<int>();
            if (Q != P)
                die(verdicts::wrong, "Guessed array not same as P");
        }

        std::swap(current, next);
    }
    
    // gracefully terminate all processes
    current.read_secret();
    current.write_secret();
    current.write(false);
    current.flush();
    next.read_secret();
    next.write_secret();
    next.write(false);
    next.flush();

    quit(verdicts::correct, "", 1.0);
    return 0;
}
