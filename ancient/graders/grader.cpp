#include "ancient.h"
#include <cstdio>
#include <vector>
#include <utility>
#include <cassert>
#include <vector>
#include <string>
#include <cstdio>
#include <signal.h>
using namespace std;
/******************************** Begin copy-pasted from IOI 25  ********************************/
namespace {

/******************************** Begin testlib-related material ********************************/
#ifdef _MSC_VER
#   define NORETURN __declspec(noreturn)
#elif defined __GNUC__
#   define NORETURN __attribute__ ((noreturn))
#else
#   define NORETURN
#endif
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


bool exit_allowed = false;

NORETURN void authorized_exit(int exit_code) {
  exit_allowed = true;
  exit(exit_code);
}

// In latest CMS, the communication defaults to stdio, but since we are still using v1.4,
// We should use fifo instead (TPS also supports fifo mode)
FILE* fin = stdin;
FILE* fout = stdout;

void out_flush() {
	fflush(fout);
}

void write_int(std::integral auto x) {
    if (1 != fwrite(&x, sizeof(x), 1, fout)) {
		fprintf(stderr, "Could not write int to fout\n");
		authorized_exit(3);
    }
}

void write_array(size_t length, auto* array) {
    if (length != fwrite(array, sizeof(decltype(*array)), length, fout)) {
		fprintf(stderr, "Could not write array to fout\n");
		authorized_exit(3);
    }
}

void write_secret(int g2m_code = G2M_CODE__OK) {
	write_int(secret_g2m | g2m_code);
}

NORETURN void die(int g2m_code) {
	if(g2m_code == G2M_CODE__OK) {
		fprintf(stderr, "Shall not die with code OK\n");
		authorized_exit(5);
	}
	fprintf(stderr, "Dying with code %d\n", g2m_code);
	if(g2m_code != G2M_CODE__SILENT)
		write_secret(g2m_code);
	fclose(fin);
	fclose(fout);
	authorized_exit(0);
}

template<std::integral T>
T read_int() {
    T x;
    if (1 != fread(&x, sizeof(x), 1, fin)) {
		fprintf(stderr, "Could not read int from stdin\n");
		authorized_exit(3);
    }
    return x;
}

void read_array(size_t length, auto* array) {
    if (length != fread(array, sizeof(decltype(*array)), length, fin)) {
		fprintf(stderr, "Could not read array from stdout\n");
		authorized_exit(3);
    }
}

void read_secret() {
	int secret = read_int<int>();
	if((secret & ~code_mask) != secret_m2g)
		die(G2M_CODE__PV_TAMPER_M2G);
	int m2g_code = secret & code_mask;
	if(m2g_code != M2G_CODE__OK)
		die(G2M_CODE__SILENT);
}

void check_exit_protocol() {
  	if (!exit_allowed)
    	die(G2M_CODE__PV_CALL_EXIT);
}

// grader logic
int n;

} // namespace
/******************************** End copy-pasted from IOI 25  ********************************/

std::vector<int> detect_convenience_values(const std::vector<long long> &t) {
    if (int(t.size()) != ::n)
        die(G2M_CODE__WA_ARRAYT);
    write_secret();
    write_int(false);
    write_array(::n, t.data());
    out_flush();
    std::vector<int> ret(n);
    read_secret();
    read_array(n, ret.data());
    return ret;
}

int main(int argc, char** argv) {
	signal(SIGPIPE, SIG_IGN);
	atexit(check_exit_protocol);
	at_quick_exit(check_exit_protocol);

    {	// Keep alive on broken pipes
        struct sigaction sa;
        sa.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &sa, NULL);
    }
	if (argc < 1) {
		fprintf(stderr, "invalid args\n");
		authorized_exit(1);
	}

    read_secret();
    int T = read_int<int>();
   
    for (int testcase = 0; testcase < T; testcase++) {

        read_secret();
        int n = read_int<int>();
        ::n = n;
        int S = read_int<int>();
        
        auto [ret_p, ret_d] = find_information(n, S);
        
        if ((int)ret_p.size() != n - 1) 
            die(G2M_CODE__WA_ARRAYP);
        if ((int)ret_d.size() != n - 1)
            die(G2M_CODE__WA_ARRAYH);

        write_secret();
        write_int(true);
        write_array(n - 1, ret_p.data());
        write_array(n - 1, ret_d.data());
        out_flush();
    }

	authorized_exit(0);
}