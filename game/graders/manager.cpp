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

// Judge strategies

int main(int argc, char **argv)
{
    // Prepare solutions
    init_communication(argc, argv);

    // Communication
    auto &leader = sol[0], &others = sol[1];
    leader.read_secret();
    others.read_secret();

    std::string scoring_mode;
    if (!(std::cin >> scoring_mode))
        quit(verdicts::judge_failure, "Cannot read manager scoring mode from input");

    std::string strategy;
    if (!(std::cin >> strategy))
        quit(verdicts::judge_failure, "Cannot read manager strategy from input");

    int L, N, M;
    bool state_hack_active = false;
    bool state_hacked = false;
    std::uint64_t state_hack_target = 0;

    if (strategy == "fixed")
    {
        if (!(std::cin >> L))
            quit(verdicts::judge_failure, "Cannot read L from input");
        if (!(std::cin >> N))
            quit(verdicts::judge_failure, "Cannot read N from input");
        if (!(std::cin >> M))
            quit(verdicts::judge_failure, "Cannot read M from input");
    }
    else if (strategy == "state-hack")
    {
        if (!(std::cin >> L))
            quit(verdicts::judge_failure, "Cannot read L from input");
        if (!(std::cin >> N))
            quit(verdicts::judge_failure, "Cannot read N from input");
        M = L + 2 * N;
    }
    else
        quit(verdicts::judge_failure, "Unknown strategy");

    leader.write_secret();
    leader.write(true);
    leader.write(L);
    leader.write(N);
    leader.write(0);
    leader.flush();

    leader.read_secret();
    int K = leader.read<int>();
    if (K < 0 || 64 < K)
        die(verdicts::wrong, "invalid K");

    struct student_state
    {
        int last_call = -1;
        int calls = 0;
        std::uint64_t before;
        std::uint64_t after;
    };
    std::uint64_t state = 0;
    std::vector<student_state> record(N + 1);

    int last_leader = -1;
    for (int i = 0; i < M; i++)
    {
        int A;
        if (strategy == "fixed")
        {
            if (!(std::cin >> A))
                quit(verdicts::judge_failure, std::format("Cannot read A_{} from input", i));
        }
        else if (strategy == "state-hack")
        {
            if (state_hack_active && i == M - 1)
                A = N, state_hack_active = false;
            else if (state_hack_active && state == state_hack_target && record[N].last_call < last_leader)
                A = N, state_hacked = true;
            else
            {
                if (!(std::cin >> A))
                    quit(verdicts::judge_failure, std::format("Cannot read A_{} from input", i));
            }
        }
        else
            quit(verdicts::judge_failure, "Unknown strategy");

        if (A == 0)
        {
            leader.write_secret();
            leader.write<char>('C');
            leader.write(state);
            leader.flush();

            leader.read_secret();
            leader.read<bool>(); // discard
            state = leader.read<uint64_t>();
            last_leader = i;
        }
        else
        {
            others.write_secret();
            others.write(true);
            others.write(L);
            others.write(N);
            others.write(A);
            others.flush();

            others.read_secret();
            int other_K = others.read<int>();
            if (other_K != K)
                die(verdicts::wrong, "inconsistent K");

            if (record[A].calls == 1)
            {
                others.write_secret();
                others.write<char>('C');
                others.write(record[A].before);
                others.flush();

                others.read_secret();
                others.read<bool>(); // discard
                if (record[A].after != others.read<uint64_t>())
                    die(verdicts::wrong, "inconsistent response");
            }
            if (record[A].calls >= 2)
                quit(verdicts::judge_failure, "other student called for more than 2 times");

            others.write_secret();
            others.write<char>('C');
            others.write(state);
            others.flush();

            others.read_secret();
            auto res = others.read<bool>();
            auto next = others.read<uint64_t>();
            if (record[A].calls == 1)
            {
                bool ans = record[A].last_call < last_leader;
                if (ans != res)
                    die(verdicts::wrong, "wrong answer reported");
                if (state_hacked)
                {
                    assert(record[A].after == state);
                    die(verdicts::wrong, "same state after leaving and after leader came in: wrong answer");
                }
            }
            others.write_secret();
            others.write<char>('R');
            others.flush();

            others.read_secret();

            record[A].calls++;
            record[A].last_call = i;
            record[A].before = state;
            record[A].after = next;

            state = next;
        }
        
        if (strategy == "state-hack" && A == N)
            state_hack_active = true, state_hack_target = state;
    }

    // gracefully terminate other things
    leader.write_secret();
    leader.write('E');
    leader.flush();
    others.write_secret();
    others.write(false);
    others.flush();

    if (scoring_mode == "binary")
        quit(verdicts::correct, "", 1.0);
    else if (scoring_mode == "partial")
    {
        if (K <= 8)
            quit(verdicts::correct, std::format("K = {}", K), 1.0);
        else
        {
            int score;
            if (21 <= K)
                quit(verdicts::wrong, std::format("K = {}", K));
            else if (13 <= K)
                score = 20;
            else if (9 <= K)
                score = 50;
            else
                quit(verdicts::judge_failure, "Reached an unreachable code!");

            quit(verdicts::partial, std::format("K = {}", K), score / 80.0);
        }
    }

    quit(verdicts::judge_failure, "Reached an unreachable code!");
    return 0;
}
