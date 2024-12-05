#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <stack>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <climits>

// Logger class for debugging and logging
class Logger
{
public:
    template <typename... Args>
    void DEBUG(Args... args)
    {
        std::ofstream debugfile("outputs/debug.txt", std::ios::app);
        std::ostringstream oss;
        (oss << ... << args); // Use fold expression
        debugfile << oss.str() << "\n";
        debugfile.close();
    }

    template <typename... Args>
    void OUTPUT(Args... args)
    {
        std::ofstream outputfile("outputs/out.txt", std::ios::app);
        std::ostringstream oss;
        (oss << ... << args); // Stream all arguments
        outputfile << oss.str() << "\n";
        outputfile.close();
    }
};

static Logger LOGGER;
long V, E;
std::vector<long> *adjacencyList;
std::vector<double> CB;

// Function to read input graph from a file
void readInput(std::string filename)
{
    std::ifstream inputfile(filename);
    inputfile >> V >> E;

    adjacencyList = new std::vector<long>[V];
    for (int i = 0; i < E; i++)
    {
        long u, v;
        inputfile >> u >> v;
        adjacencyList[u].push_back(v);
    }
    inputfile.close();
}

// Thread-safe function to compute betweenness centrality for a single source node
void brandes(int s, std::vector<double> &localCB)
{
    std::vector<double> delta(V, 0.0);
    std::vector<std::vector<long>> prev(V);
    std::vector<long> sigma(V, 0);
    std::vector<long> dist(V, LLONG_MAX);

    sigma[s] = 1;
    dist[s] = 0;

    std::queue<long> queue;
    queue.push(s);
    std::stack<long> stack;

    // Forward BFS to compute shortest paths
    while (!queue.empty())
    {
        long u = queue.front();
        queue.pop();
        stack.push(u);

        for (auto v : adjacencyList[u])
        {
            if (dist[v] == LLONG_MAX)
            {
                dist[v] = dist[u] + 1;
                queue.push(v);
            }

            if (dist[v] == dist[u] + 1)
            {
                sigma[v] += sigma[u];
                prev[v].push_back(u);
            }
        }
    }

    // Backward phase to compute dependencies
    while (!stack.empty())
    {
        int v = stack.top();
        stack.pop();

        for (auto u : prev[v])
        {
            delta[u] += (double(sigma[u]) / sigma[v]) * (1.0 + delta[v]);
        }

        if (v != s)
        {
            localCB[v] += delta[v];
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./program <input_file>\n";
        return 1;
    }

    readInput(argv[1]);
    CB.resize(V, 0.0);

    auto start_time = std::chrono::high_resolution_clock::now();
    LOGGER.OUTPUT("The start time is ", std::chrono::system_clock::to_time_t(start_time));

    // Launch threads
    std::vector<std::thread> threads;
    std::vector<std::vector<double>> localCB(V, std::vector<double>(V, 0.0)); // Per-thread storage for CB
    for (int s = 0; s < V; s++)
    {
        threads.emplace_back(brandes, s, std::ref(localCB[s]));
    }

    // Join threads
    for (auto &th : threads)
    {
        th.join();
    }

    // Aggregate results from all threads
    for (int s = 0; s < V; s++)
    {
        for (int v = 0; v < V; v++)
        {
            CB[v] += localCB[s][v];
        }
    }

    // Output the centrality values
    for (int i = 0; i < V; i++)
    {
        std::cout << CB[i] * (1.0 / ((V - 1) * (V - 2))) << " ";
    }
    std::cout << "\n";

    auto stop_time = std::chrono::high_resolution_clock::now();
    LOGGER.OUTPUT("The stop time is ", std::chrono::system_clock::to_time_t(stop_time));

    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
    LOGGER.OUTPUT("Total execution time for parallelized version: ", time_diff, " milliseconds");

    return 0;
}
