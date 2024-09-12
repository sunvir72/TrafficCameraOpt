#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <pthread.h>
#include <string>
#include <cstdlib>
#include <time.h>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include "set"
#include <sys/wait.h>
#include <unordered_set>

using namespace std;

class Edge {
public:
    int from, to;

    Edge(int from, int to) : from(from), to(to) {}

    string toString() const {
        string result = "<";
        result += to_string(from + 1);
        result += ",";
        result += to_string(to + 1);
        result += ">";
        return result;
    }
};

class Graph
{
public:
    double time_cnf, time_app_vc1, time_app_vc2;
    vector<int> CNF_SAT_VC;
    vector<int> APPROX_VC_1;
    vector<int> APPROX_VC_2;
    bool timeout_CNFSATVC = false;
};

vector<Edge> edges;
vector<Graph> graphs;
Graph graph;
// unordered_set<int> verticesPresentInEdges;

// vector<int> solveAndGetKCover(int n, int k)
// {

//     unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
//     vector<vector<Minisat::Lit>> atomic_propositions(n);

//     vector<int> vt_verticesPresentInEdges(verticesPresentInEdges.begin(), verticesPresentInEdges.end());

//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < k; j++)
//         {
//             atomic_propositions[i].push_back(Minisat::mkLit(solver->newVar()));
//         }
//     }

//     for (int vc_pos = 0; vc_pos < k; vc_pos++)
//     {
//         Minisat::vec<Minisat::Lit> clause_atl_1V_in_VC;
//         for (vector<int>::size_type vert_it = 0; vert_it < vt_verticesPresentInEdges.size(); vert_it++)
//         {
//             clause_atl_1V_in_VC.push(atomic_propositions[vt_verticesPresentInEdges[vert_it]][vc_pos]);
//         }
//         solver->addClause(clause_atl_1V_in_VC);
//     }

//     for (int m = 0; m < k; m++)
//     {
//         for (vector<int>::size_type p = 0; p < vt_verticesPresentInEdges.size() - 1; p++)
//         {
//             for (vector<int>::size_type q = p + 1; q < vt_verticesPresentInEdges.size(); q++)
//             {
//                 solver->addClause(~atomic_propositions[vt_verticesPresentInEdges[p]][m], ~atomic_propositions[vt_verticesPresentInEdges[q]][m]);
//             }
//         }
//     }

//     for (vector<int>::size_type vert_it = 0; vert_it < vt_verticesPresentInEdges.size(); vert_it++)
//     {
//         for (int p = 0; p < k - 1; p++)
//         {
//             for (int q = p + 1; q < k; q++)
//             {
//                 solver->addClause(~atomic_propositions[vt_verticesPresentInEdges[vert_it]][p], ~atomic_propositions[vt_verticesPresentInEdges[vert_it]][q]);
//             }
//         }
//     }

//     for (size_t e = 0; e < edges.size(); e++)
//     {
//         Minisat::vec<Minisat::Lit> clause_every_edge_incTo_atleast_1_v_in_VC;
//         for (int p = 0; p < k; p++)
//         {
//             clause_every_edge_incTo_atleast_1_v_in_VC.push(atomic_propositions[edges[e].from][p]);
//             clause_every_edge_incTo_atleast_1_v_in_VC.push(atomic_propositions[edges[e].to][p]);
//         }
//         solver->addClause(clause_every_edge_incTo_atleast_1_v_in_VC);
//     }

//     vector<int> vertex_cover;
//     if (solver->solve())
//     {
//         for (int i = 0; i < n; i++)
//         {
//             for (int j = 0; j < k; j++)
//             {
//                 if (solver->modelValue(atomic_propositions[i][j]) == Minisat::l_True)
//                     vertex_cover.push_back(i + 1);
//             }
//         }
//     }
//     return vertex_cover;
// }

vector<int> solveAndGetKCover(int n, int k)
{

    unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    vector<vector<Minisat::Lit>> atomic_propositions(n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
        {
            atomic_propositions[i].push_back(Minisat::mkLit(solver->newVar()));
        }
    }

    for (int vc_pos = 0; vc_pos < k; vc_pos++)
    {
        Minisat::vec<Minisat::Lit> clause_atl_1V_in_VC;
        for (int vert_it = 0; vert_it < n; vert_it++)
        {
            clause_atl_1V_in_VC.push(atomic_propositions[vert_it][vc_pos]);
        }
        solver->addClause(clause_atl_1V_in_VC);
    }

    for (int m = 0; m < k; m++)
    {
        for (int p = 0; p < n - 1; p++)
        {
            for (int q = p + 1; q < n; q++)
            {
                solver->addClause(~atomic_propositions[p][m], ~atomic_propositions[q][m]);
            }
        }
    }

    for (int vert_it = 0; vert_it < n; vert_it++)
    {
        for (int p = 0; p < k - 1; p++)
        {
            for (int q = p + 1; q < k; q++)
            {
                solver->addClause(~atomic_propositions[vert_it][p], ~atomic_propositions[vert_it][q]);
            }
        }
    }

    for (size_t e = 0; e < edges.size(); e++)
    {
        Minisat::vec<Minisat::Lit> clause_every_edge_incTo_atleast_1_v_in_VC;
        for (int p = 0; p < k; p++)
        {
            clause_every_edge_incTo_atleast_1_v_in_VC.push(atomic_propositions[edges[e].from][p]);
            clause_every_edge_incTo_atleast_1_v_in_VC.push(atomic_propositions[edges[e].to][p]);
        }
        solver->addClause(clause_every_edge_incTo_atleast_1_v_in_VC);
    }

    vector<int> vertex_cover;
    if (solver->solve())
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < k; j++)
            {
                if (solver->modelValue(atomic_propositions[i][j]) == Minisat::l_True)
                    vertex_cover.push_back(i + 1);
            }
        }
    }
    return vertex_cover;
}


void *solve_and_get_cnf_sat_vc(void *args)
{
    clockid_t appVC1;
    pthread_getcpuclockid(pthread_self(), &appVC1);
    struct timespec start, end;
    clock_gettime(appVC1, &start);

    int *V = (int *)args;
    vector<int> vertex_cover_curr;
    vector<int> vertex_cover_ans;
    int hi = *V;
    int lo = 1;

    while (lo <= hi)
    {
        int mid = lo + (hi - lo) / 2;
        vertex_cover_curr = solveAndGetKCover(*V, mid);
        if (!vertex_cover_curr.empty())
        {
            vertex_cover_ans = vertex_cover_curr;
            hi = mid - 1;
        }
        else
        {
            lo = mid + 1;
        }
    }

    if (vertex_cover_ans.empty())
    {
        vertex_cover_ans.emplace_back(-1);
    }
    else
    {
        sort(vertex_cover_ans.begin(), vertex_cover_ans.end());
    }

    graph.CNF_SAT_VC = vertex_cover_ans;
    clock_gettime(appVC1, &end);
    double cpuTime = difftime(end.tv_sec, start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
    graph.time_cnf = cpuTime;
    return NULL;
}

void *solve_and_get_approx_vc_1(void *args)
{
    clockid_t appVC1;
    pthread_getcpuclockid(pthread_self(), &appVC1);
    struct timespec start;
    clock_gettime(appVC1, &start);

    int *V = (int *)args;
    vector<int> vertex_cover_ans;
    vector<Edge> tempEdges(edges.begin(), edges.end());

    while (!tempEdges.empty())
    {
        vector<int> degree(*V, 0);
        int maxDegree = -1;
        int vertex = -1;

        for (size_t i = 0; i < tempEdges.size(); ++i)
        {
            degree[tempEdges[i].from]++;
            degree[tempEdges[i].to]++;
            if (degree[tempEdges[i].from] > maxDegree)
            {
                maxDegree = degree[tempEdges[i].from];
                vertex = tempEdges[i].from;
            }
            if (degree[tempEdges[i].to] > maxDegree)
            {
                maxDegree = degree[tempEdges[i].to];
                vertex = tempEdges[i].to;
            }
        }

        vertex_cover_ans.emplace_back(vertex + 1);

        for (auto it = tempEdges.begin(); it != tempEdges.end();)
        {
            if (it->from == vertex || it->to == vertex)
            {
                it = tempEdges.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    sort(vertex_cover_ans.begin(), vertex_cover_ans.end());
    graph.APPROX_VC_1 = vertex_cover_ans;

    struct timespec end;
    clock_gettime(appVC1, &end);

    double cpuTime = static_cast<double>(end.tv_sec - start.tv_sec) + static_cast<double>(end.tv_nsec - start.tv_nsec) * 1e-9;
    graph.time_app_vc1 = cpuTime;

    return NULL;
}

void *solve_and_get_approx_vc_2(void *args)
{
    clockid_t appVC1;
    pthread_getcpuclockid(pthread_self(), &appVC1);
    struct timespec start;
    clock_gettime(appVC1, &start);

    vector<int> vertex_cover_ans;
    vector<Edge> edgesCopy(edges.begin(), edges.end());

    while (!edgesCopy.empty())
    {
        Edge edge = edgesCopy[0];
        int from = edge.from;
        int to = edge.to;
        vertex_cover_ans.emplace_back(from + 1);
        vertex_cover_ans.emplace_back(to + 1);

        for (auto it = edgesCopy.begin(); it != edgesCopy.end();)
        {
            if (it->from == from || it->to == from || it->from == to || it->to == to)
            {
                it = edgesCopy.erase(it); 
            }
            else
            {
                ++it; 
            }
        }
    }

    sort(vertex_cover_ans.begin(), vertex_cover_ans.end());

    graph.APPROX_VC_2 = vertex_cover_ans;

    struct timespec end;
    clock_gettime(appVC1, &end);

    double cpuTime = static_cast<double>(end.tv_sec - start.tv_sec) + static_cast<double>(end.tv_nsec - start.tv_nsec) * 1e-9;
    graph.time_app_vc2 = cpuTime;

    return NULL;
}

void getKCovers(int V) {
    pthread_t thread_cnf_sat, thread_approx_vc_1, thread_approx_vc_2;
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 120;

    pthread_create(&thread_cnf_sat, NULL, solve_and_get_cnf_sat_vc, (void *)&V);
    pthread_create(&thread_approx_vc_1, NULL, solve_and_get_approx_vc_1, (void *)&V);
    pthread_create(&thread_approx_vc_2, NULL, solve_and_get_approx_vc_2, (void *)&V);
    int result = pthread_timedjoin_np(thread_cnf_sat, nullptr, &timeout);
    if(result == ETIMEDOUT) {
        graph.timeout_CNFSATVC = true;
    }
    pthread_join(thread_approx_vc_1, NULL);
    pthread_join(thread_approx_vc_2, NULL);
    
    graphs.emplace_back(graph);
}

void printOutput()
{
    for (const auto &g : graphs)
    {
        if (g.timeout_CNFSATVC)
        {
            cout << "CNF-SAT-VC: timeout" << endl;
        }
        else
        {
            ostringstream ansCnfSat;
            for (size_t i = 0; i < g.CNF_SAT_VC.size(); i++)
            {
                ansCnfSat << g.CNF_SAT_VC[i];
                if (i < g.CNF_SAT_VC.size() - 1)
                {
                    ansCnfSat << ",";
                }
            }
            cout << "CNF-SAT-VC: " << ansCnfSat.str() << endl;
        }

        ostringstream ansVc1;
        for (size_t i = 0; i < g.APPROX_VC_1.size(); i++)
        {
            ansVc1 << g.APPROX_VC_1[i];
            if (i < g.APPROX_VC_1.size() - 1)
            {   
                ansVc1 << ",";
            }
        }
        cout << "APPROX-VC-1: " << ansVc1.str() << endl ;

        ostringstream ansVc2;
        for (size_t i = 0; i < g.APPROX_VC_2.size(); i++)
        {
            ansVc2 << g.APPROX_VC_2[i];
            if (i < g.APPROX_VC_2.size() - 1)
            {
                 ansVc2 << ",";
            }
        }
        cout << "APPROX-VC-2: " << ansVc2.str() << endl;
    }
}

void constructEdges(const string &input, int V)
{
    size_t start = input.find('<');
    size_t end;
    while (start != string::npos)
    {
        end = input.find('>', start);
        string pairStr = input.substr(start + 1, end - start - 1);
        size_t commaPos = pairStr.find(',');
        int from = stoi(pairStr.substr(0, commaPos)) - 1;
        int to = stoi(pairStr.substr(commaPos + 1)) - 1;

        if (from >= 0 && from < V && to >= 0 && to < V)
        {
            edges.emplace_back(from, to);
            // verticesPresentInEdges.insert(from);
            // verticesPresentInEdges.insert(to);
        }
        else
        {
            edges.clear();
            cerr << "Error: One or more vertices in the edge set are not present in the graph." << endl;
            break;
        }

        start = input.find('<', end);
    }

    if (not edges.empty())
    {
        getKCovers(V);
    }
    else
    {
        cerr << "Error: Edges are not defined" << endl;
    }
}

void *getAndParseInput(void *arg)
{
    int V = -1;
    string inputCommand;
    while (!cin.eof())
    {
        try
        {
            string line;
            getline(cin, line);
            istringstream input(line);
            if (line.empty())
            {
                continue;
            }
            if (input.eof())
                break;
            input >> inputCommand;
            if (inputCommand == "V")
            {
                edges.clear();
                input >> V;
                if (V <= 0)
                {
                    cerr << "Number of vertices must be positive" << endl;
                }
                edges.clear();
            }
            else if (inputCommand == "E")
            {
                constructEdges(line, V);
            }
            // TODO: Remove
            // else if (inputCommand == "B")
            // {
            //     break;
            // }

            else
            {
                cerr << "Error: Unrecognized command or sequence of commands" << endl;
            }
        }
        catch (...)
        {
            cerr << "Error: Something went wrong. Try re-running the program" << endl;
        }
    }

    printOutput();
    return NULL;
}

int main()
{
    pthread_t ioThread;
    pthread_create(&ioThread, nullptr, getAndParseInput, NULL);
    pthread_join(ioThread, NULL);
}