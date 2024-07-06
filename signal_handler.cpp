#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <signal.h>
#include <vector>

extern std::vector<int> process_ids;

void handleSignal(int signum) {
    for (pid_t pId : process_ids) {
        kill(pId, SIGTERM);
    }
    exit(signum);
}
