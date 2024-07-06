#include <iostream>
#include <sstream>
#include <vector>
#include <queue>

using namespace std;

void printPath(vector<int> parentVertex, int end) {
    int vertex = end;
    string shortestPath = "";
    while(vertex != -1) {
        shortestPath = to_string(vertex) + "-" + shortestPath;
        vertex = parentVertex[vertex];
    }
    if (!shortestPath.empty()) {
        shortestPath.pop_back();
    }
    cout<<shortestPath<<endl;
}

void generateShortestPath(int start, int end, int V, vector<vector<int>>& edges) {
    if (!(start >= 1 && start <= V && end >= 1 && end <= V)) {
        cerr << "Error: Start or End vertex out of bounds" << endl;
        return;
    }
    if(!edges.empty()) {
        if(start == end) {
            cout << start << endl;
            return;
        }
        vector<int> parentVertex(V + 1, -1);
        vector<bool> visited(V + 1);
        queue<int> que;
        que.push(start);
        visited[start] = true;
        while(!que.empty()) {
            int front = que.front();
            que.pop();

            if(front == end) {
                break;
            }

            vector<int> adjascents = edges[front];
            for(int adjascent: adjascents) {
                if(!visited[adjascent]) {
                    visited[adjascent] = true;
                    que.push(adjascent);
                    parentVertex[adjascent] = front;
                }
            }
        }
        
        if(parentVertex[end] != -1) {
            printPath(parentVertex, end);
        }
        else {
            cerr << "Error: No path found from the source to the destination." << endl;   
        }
    } else {
        cerr << "Error: No edges found. Make sure vertices and edges have been correctly specified" << endl;
    }
}

void constructEdges(const string& input, int V, vector<vector<int>>& edges) {
    for(int i = 0; i <= V; i++) {
        edges.emplace_back();
    }
    size_t start = input.find('<');
    size_t end;
    while (start != std::string::npos) {
        end = input.find('>', start);
        std::string pairStr = input.substr(start + 1, end - start - 1);
        size_t commaPos = pairStr.find(',');
        int from = std::stoi(pairStr.substr(0, commaPos));
        int to = std::stoi(pairStr.substr(commaPos + 1));
        
        if(from >= 1 && from <= V && to >= 1 && to <= V) {
                edges[from].push_back(to);
                edges[to].push_back(from);
            }
            else {
                edges.clear();
                cerr << "Error: One or more vertices in the edge set are not present in the graph." << endl;
                break;
            }
            
        start = input.find('<', end);
    }
}

int main() {
    int V = -1;
    string inputCommand;
    string previousCommand;
    vector<vector<int>> edges;

    while (!cin.eof()) {
        try {
            string line;
            getline(cin, line);
            istringstream input(line);
            cout<<"Here "<<line<<endl;
            if(line.empty()) {
                continue;
            }
            if(input.eof()) {
                break;
            }
            if(!inputCommand.empty()) {
                previousCommand = inputCommand;
            }
            input >> inputCommand;

            if (inputCommand == "s") {
                int start, end;
                input >> start >> end;
                generateShortestPath(start, end, V, edges);
            }
            else if(inputCommand == "V") {
                input >> V;
                if(V <= 0) {
                    cerr << "Error: Number of vertices must be positive" << endl;
                }
                cout<<line<<endl;
                edges.clear();//TODO: Move to constructEdges
            } else if(inputCommand == "E") {
                if (previousCommand != "V") {
                    cerr << "Error: Edges must follow vertices in input" << endl;
                } else {
                    cout<<line<<endl;
                    constructEdges(line, V, edges);
                }
            } else {
                cerr << "Error: Unrecognized command or sequence of commands" << endl;
            }
        }
        catch(...) {
            cerr << "Error: Something went wrong. Try re-running the program" << endl;
        }
    }
}