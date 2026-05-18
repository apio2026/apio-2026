#include "game.h"

#include <csignal>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include <concepts>
#include <cstring>
#include <string>
#include <tuple>
#include <typeinfo>
#include <unistd.h>
#include <vector>

// In case of using FIFO, remove this define
#define GRADER_USE_STDIO

namespace
{
    bool exit_allowed = false;
    [[noreturn]] void authorized_exit(int exit_code)
    {
        exit_allowed = true;
        std::exit(exit_code);
    }

    constexpr int NUM_PROCS = 2;

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

#ifdef GRADER_USE_STDIO
    FILE *fin = stdin;
    FILE *fout = stdout;
#else
    FILE *fin = nullptr;
    FILE *fout = nullptr;
#endif

    void out_flush()
    {
        fflush(fout);
    }

    void close_stream(FILE *&f)
    {
        if (f)
            fclose(f);
        f = nullptr;
    }

    void write_int(std::integral auto x)
    {
        if (fwrite(reinterpret_cast<char *>(&x), sizeof(x), 1, fout) != 1)
        {
            fprintf(stderr, "Could not write %s to fout\n", typeid(x).name());
            authorized_exit(3);
        }
    }

    void write_secret(int g2m_code = G2M_CODE_OK)
    {
        write_int(secret_g2m | g2m_code);
    }

    [[noreturn]] void die(int g2m_code)
    {
        if (g2m_code == G2M_CODE_OK)
        {
            fprintf(stderr, "Shall not die with code OK\n");
            authorized_exit(5);
        }
        fprintf(stderr, "Dying with code %d\n", g2m_code);
        if (g2m_code != G2M_CODE_SILENT)
            write_secret(g2m_code);
        close_stream(fout);
        close_stream(fin);
        authorized_exit(0);
    }

    template <std::integral I>
    I read_int()
    {
        I x;
        if (fread(reinterpret_cast<char *>(&x), sizeof(x), 1, fin) != 1)
        {
            fprintf(stderr, "Could not read %s from fin\n", typeid(x).name());
            authorized_exit(3);
        }
        return x;
    }

    void read_secret()
    {
        int secret = read_int<int>();
        if ((secret & ~code_mask) != secret_m2g)
            die(G2M_CODE_PV_TAMPER_M2G);
        int m2g_code = secret & code_mask;
        if (m2g_code != M2G_CODE_OK)
            die(G2M_CODE_SILENT);
    }

    void check_exit_protocol()
    {
        if (!exit_allowed)
            die(G2M_CODE_PV_CALL_EXIT);
    }

} // namespace

namespace
{
    std::vector<bool> to_array(std::uint64_t state, int K)
    {
        std::vector<bool> res(K);
        for (int i = 0; i < K; i++)
            res[i] = state & (1ull << i);
        return res;
    }
    std::uint64_t from_array(std::vector<bool> arr)
    {
        std::uint64_t state = 0;
        for (int i = 0; i < (int)arr.size(); i++)
            state ^= (std::uint64_t)arr[i] << i;
        return state;
    }
}

int main(int argc, char **argv)
{
    std::setvbuf(stdin, nullptr, _IONBF, 0);

    signal(SIGPIPE, SIG_IGN);
    atexit(check_exit_protocol);
    at_quick_exit(check_exit_protocol);

    // Grader logic
    write_secret();
    out_flush();

    read_secret();
    bool cont = read_int<bool>();
    if (!cont)
        authorized_exit(0);
    int L = read_int<int>();
    int N = read_int<int>();
    int id = read_int<int>();

    int K = start_game(L, N, id);
    write_secret();
    write_int(K);
    out_flush();

    int calls = 0;
    char mode;
    while (read_secret(), (mode = read_int<char>()))
    {
        if (mode == 'C') // call
        {
            std::uint64_t state = read_int<std::uint64_t>();
            auto c = to_array(state, K);

            std::vector<bool> ret;
            bool ans = false;
            if (id == 0)
                ret = leader(c);
            else if (id > 0 && calls == 0)
                ret = student_first(c);
            else if (id > 0 && calls == 1)
                std::tie(ans, ret) = student_second(c);
            else
                assert(false);

            if ((int)ret.size() != K)
            {
                write_secret(G2M_CODE_WA_INVALID_LENGTH);
                authorized_exit(0);
            }

            state = from_array(ret);
            write_secret();
            write_int(ans);
            write_int(state);
            out_flush();
            calls++;
        }
        else if (mode == 'R') // restart
            break;
        else if (mode == 'E') // restart
            authorized_exit(0);
    }

    if (access("/proc/self/exe", F_OK) == 0)
        execv("/proc/self/exe", argv);
    write_secret(G2M_CODE_CANNOT_EXECV);
    out_flush();
    authorized_exit(0);
}
