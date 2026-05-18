#include "testlib.h"
#include <string>
#include <cstdio>
#include <vector>
#include <signal.h>
using namespace std;

/******************************** Begin copy-pasted from IOI 25  ********************************/
/******************************** Begin testlib-related material ********************************/

inline FILE* openFile(const char* name, const char* mode) {
    FILE* file = fopen(name, mode);
    if (!file)
        quitf(_fail, "Could not open file '%s' with mode '%s'.", name, mode);
    closeOnHalt(file);
    return file;
}


vector<FILE*> mgr2sol, sol2mgr;
FILE* log_file = stderr;

void nullifyFile(int idx) {
    mgr2sol[idx] = sol2mgr[idx] = nullptr;
}

#ifdef __GNUC__
__attribute__ ((format (printf, 1, 2)))
#endif
void log_printf(const char* fmt, ...) {
    if (log_file) {
        FMT_TO_RESULT(fmt, fmt, message);
        fprintf(log_file, "%s", message.c_str());
        fflush(log_file);
    }
}

void registerManager(std::string probName, int num_processes, int argc, char* argv[]) {
    setName("manager for problem %s", probName.c_str());
    __testlib_ensuresPreconditions();
    testlibMode = _checker;
    random_t::version = 1; // Random generator version
    __testlib_set_binary(stdin);
    ouf.mode = _output;

    {//Keep alive on broken pipes
        //signal(SIGPIPE, SIG_IGN);
        struct sigaction sa;
        sa.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &sa, NULL);
    }

    int required_args = 1 + 2 * num_processes;
    if (argc < required_args || required_args+1 < argc) {
        string usage = format("'%s'", argv[0]);
        for (int i = 0; i < num_processes; i++)
            usage += format(" sol%d-to-mgr mgr-to-sol%d", i, i);
        usage += " [mgr_log] < input-file";
        quitf(_fail,
            "Manager for problem %s:\n"
            "Invalid number of arguments: %d\n"
            "Usage: %s",
            probName.c_str(), argc-1, usage.c_str());
    }

    inf.init(stdin, _input);
    closeOnHalt(stdout);
    closeOnHalt(stderr);

    mgr2sol.resize(num_processes);
    sol2mgr.resize(num_processes);
    for (int i = 0; i < num_processes; i++) {
        mgr2sol[i] = openFile(argv[1 + 2*i + 1], "a");
        sol2mgr[i] = openFile(argv[1 + 2*i + 0], "r");
    }

    if (argc > required_args) {
        log_file = openFile(argv[required_args], "w");
    } else {
        log_file = nullptr;
    }
}
/********************************* End testlib-related material *********************************/

// grader/manager protocol

const int secret_g2m = 0xA9163A60;
const int secret_m2g = 0XCB78C7A0;
const int code_mask  = 0x0000000F;

const int M2G_CODE__OK = 0;
const int M2G_CODE__DIE = 1;

const int G2M_CODE__OK = 0;
const int G2M_CODE__WA_ARRAYT = 10;
const int G2M_CODE__WA_ARRAYP = 11;
const int G2M_CODE__WA_ARRAYH = 12;
const int G2M_CODE__PV_CALL_EXIT = 13;
const int G2M_CODE__PV_TAMPER_M2G = 14;
const int G2M_CODE__SILENT = 15;

int fifo_idx = 0;

void out_flush() {
    fflush(mgr2sol[fifo_idx]);
}

void write_int(std::integral auto x) {
    FILE* fout = mgr2sol[fifo_idx];
    if (1 != fwrite(&x, sizeof(x), 1, fout)) {
        nullifyFile(fifo_idx);
        log_printf("Could not write int to mgr2sol[%d]\n", fifo_idx);
    }
}

void write_array(size_t length, auto* array) {
    FILE* fout = mgr2sol[fifo_idx];
    if (int ret = fwrite(array, sizeof(decltype(*array)), length, fout); length != ret) {
        nullifyFile(fifo_idx);
        log_printf("Could not write int array of size %d to mgr2sol[%d], fwrite returned %d\n", length, fifo_idx, ret);
    }
}

void write_secret(int m2g_code = M2G_CODE__OK) {
    write_int(secret_m2g | m2g_code);
}

#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
NORETURN void die(TResult result, const char* format, ...) {
    FMT_TO_RESULT(format, format, message);
    log_printf("Dying with message '%s'\n", message.c_str());
    for (int i = 0; i < (int)mgr2sol.size(); ++i)
        if(mgr2sol[i] != nullptr) {
            fifo_idx = i;
            log_printf("Sending secret with code DIE to mgr2sol[%d]\n", fifo_idx);
            write_secret(M2G_CODE__DIE);
            out_flush();
        }
    log_printf("Quitting with result code %d\n", int(result));
    quit(result, message);
}

/*NORETURN void die_invalid_argument(const string &msg) {
    RESULT_MESSAGE_WRONG += ": Invalid argument";
    die(_wa, "%s", msg.c_str());
}*/

NORETURN void die_rte(const string &msg) {
    RESULT_MESSAGE_WRONG = "Runtime Error";
    die(_wa, "%s", msg.c_str());
}

template<std::integral T>
T read_int() {
    FILE* fin = sol2mgr[fifo_idx];
    T x;
    if (1 != fread(&x, sizeof(x), 1, fin)) {
        nullifyFile(fifo_idx);
        die_rte("manual RTE, cant read int from grader");
    }
    return x;
}

void read_array(size_t length, auto* array) {
    FILE* fin = sol2mgr[fifo_idx];
    
    if (length != fread(array, sizeof(decltype(*array)), length, fin)) {
        nullifyFile(fifo_idx);
        die_rte("manual RTE, cant read array from grader");
    }
}

void read_secret() {
    int secret = read_int<int>();
    if((secret & ~code_mask) != secret_g2m)
        die(_pv, "Possible tampering with sol2mgr[%d]", fifo_idx);
    int g2m_code = secret & code_mask;
    switch (g2m_code) {
        case G2M_CODE__OK:
            return;
        case G2M_CODE__WA_ARRAYT:
            die(_wa, "Array t does not satisfy the constraints");
        case G2M_CODE__WA_ARRAYP:
            die(_wa, "Array p has wrong length");
        case G2M_CODE__WA_ARRAYH:
            die(_wa, "Array h has wrong length");
        case G2M_CODE__SILENT:
            die(_fail, "Unexpected g2m_code SILENT from sol2mgr[%d]", fifo_idx);
        case G2M_CODE__PV_TAMPER_M2G:
            die(_pv, "Possible tampering with mgr2sol[%d]", fifo_idx);
        case G2M_CODE__PV_CALL_EXIT:
            die(_pv, "Solution[%d] called exit()", fifo_idx);
        default:
            die(_fail, "Unknown g2m_code %d from sol2mgr[%d]", g2m_code, fifo_idx);
    }
}

/******************************** End copy-pasted from IOI 25  ********************************/

namespace {
    const int MAXQ = 700;
    const long long QUERY_C = 1'000'000'000'000'000'000;
}

int main(int argc, char **argv) {
    registerManager("ancient", 1, argc, argv);
    int max_q = 0;
    int subtask = inf.readInt();
    int T = inf.readInt();
    
    write_secret();
    write_int(T);
    out_flush();
    for (int testcase = 0; testcase < T; testcase++) {
        int qcnt = 0;
        int n = inf.readInt();
        int L = inf.readInt();
        std::vector<int> p(n);
        std::vector<int> h(n);
        for (int i = 1; i < n; i++)
            p[i] = inf.readInt();
        for (int i = 0; i < n; i++)
            h[i] = inf.readInt();

        write_secret();
        write_int(n);
        write_int(L);
        out_flush();

        while (true) {
            read_secret();
            bool is_answer = read_int<bool>();
            if (is_answer) break;

            vector<long long> t(n);
            read_array(n, t.data());
            for (int i = 0; i < n; i++)
                if (t[i] < -QUERY_C || t[i] > QUERY_C)
                    die(_wa, "invalid call");
            if (qcnt >= MAXQ)
                die(_wa, "too many queries");
            qcnt++;
            std::vector<int> ret(n);
            for (int i = 1; i < n; i++) {
                if (h[p[i]] + t[p[i]] <= h[i] + t[i]) ret[i]++;
                else ret[p[i]]++;
            }
            write_secret();
            write_array(n, ret.data());
            out_flush();
        }
        vector<int> ret_p(n - 1);
        vector<int> ret_d(n - 1);
        
        read_array(n - 1, ret_p.data());
        read_array(n - 1, ret_d.data());

        for (int i = 1; i < n; i++) {
            if (ret_p[i - 1] != p[i])
                die(_wa, "incorrect answer");
            if (ret_d[i - 1] != h[i] - h[0])
                die(_wa, "incorrect answer");
        }
        max_q = std::max(max_q, qcnt);
    }

    if (max_q > 700)
        quit(_wa);

    if (subtask == 0)
        quitf(_ok, ("q = " + std::to_string(max_q)).c_str());

    if (max_q > 38) {
        quitp((double) std::max(1.0/5.5, 1 + std::log(18) / std::log(7.5) - std::log(max_q - 20) / std::log(7.5)), "q = " + std::to_string(max_q));
    }

    quitf(_ok, ("q = " + std::to_string(max_q)).c_str());
}
