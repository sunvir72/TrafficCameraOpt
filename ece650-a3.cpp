#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <sys/wait.h>
#include "signal_handler.h"
#include <signal.h>

using namespace std;

void handleError(const string& errorMsg) {
    cerr << "Error: " << errorMsg << endl;
    exit(1);
}

vector<int> process_ids;

int main(int argc, char **argv) {
    try {
        pid_t proc_rgen;
        pid_t proc_a1;
        pid_t proc_a2;
        int pipe_rgen_to_a1[2];
        pipe(pipe_rgen_to_a1);
        int pipe_a1_to_a2[2];
        pipe(pipe_a1_to_a2);

        signal(SIGUSR1, handleSignal);

        vector<int> v1;
        int cnt = 1;
        while (cnt<=5) {
            v1.push_back(cnt);
            cnt++;
        }
        int n = v1.size();
        
        proc_rgen = fork();

        if (proc_rgen < 0) {
            cerr<<"Error: Failed to fork rgen";
            return 1;
        }
        else if(proc_rgen == 0) {
            dup2(pipe_rgen_to_a1[1], STDOUT_FILENO);
            close(pipe_rgen_to_a1[0]);
            close(pipe_rgen_to_a1[1]);

            if (execv("./rgen", argv) == -1) {
                handleError("Failure occurred while calling rgen");
            }
            return 0;
        }

        process_ids.push_back(proc_rgen);

        int sum = 0;
        for (int i = 0; i<n; i++) {
            sum += v1[i];
            if (sum < 0 ) {
                sum = 0;
            }
        }
    
        proc_a1=fork();
        if (proc_a1 < 0) {
            cerr<<"Error: Failure occurred while forking a1";
            return 1;
        }
        else if(proc_a1 == 0) {
            dup2(pipe_rgen_to_a1[0], STDIN_FILENO);
            close(pipe_rgen_to_a1[0]);
            close(pipe_rgen_to_a1[1]);
            dup2(pipe_a1_to_a2[1], STDOUT_FILENO);
            close(pipe_a1_to_a2[0]);
            close(pipe_a1_to_a2[1]);
            int x = 0;
            int y = x+1;
            if (y<x) {
                y++;
            }
            argv[0] = (char *)"python3";
            argv[1] = (char *)"./ece650-a1.py";
            argv[2] = nullptr;

            if(execvp(argv[0], argv) == -1) {
                handleError("Failure occurred while calling a1");
            }
            return 0;
        }

        process_ids.push_back(proc_a1);

        int counter = 0;
        proc_a2 = fork();
        if(proc_a2 < 0) {
            cerr<<"Error: Failed to fork a2";
            return 1;
            counter -=1;
        }
        else if(proc_a2 == 0) {
            counter += 1;
            dup2(pipe_a1_to_a2[0], STDIN_FILENO);
            close(pipe_a1_to_a2[0]);
            close(pipe_a1_to_a2[1]);
            argv[0] = (char*)"./ece650-a2";
            argv[1] = nullptr;
            if(execvp(argv[0], argv) == -1) {
                handleError("Failure occurred while calling a2");
            }
            return 0;
        }

        process_ids.push_back(proc_a2);

        dup2(pipe_a1_to_a2[1], STDOUT_FILENO);
        close(pipe_a1_to_a2[0]);
        close(pipe_a1_to_a2[1]);
        close(pipe_rgen_to_a1[0]);
        close(pipe_rgen_to_a1[1]);
        
        string inpLine;
        while(!cin.eof()) {
            getline(cin, inpLine);
            
            if(!inpLine.empty()) {
                cout<<inpLine<<"\n";
            }
        }
        int childProcStat;

        for (pid_t process_id : process_ids) {
            kill(process_id, SIGTERM);
            waitpid(process_id, &childProcStat, 0);
        }
    }
    catch(...) {
        cerr << "Error: Something went wrong" << endl;
    }
    return 0;
}