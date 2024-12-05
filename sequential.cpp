#include <iostream>
#include <vector>
#include <climits>
#include <chrono>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>

// Logger class for printing and logging to log files and debugging
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

// Function to convert time_point to a formatted string
std::string getSysTime(const std::chrono::high_resolution_clock::time_point &tp)
{
    auto time_t = std::chrono::high_resolution_clock::to_time_t(tp);
    std::tm *tm = std::localtime(&time_t); // Convert to local time
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", tm);
    return std::string(buffer);
}

static Logger LOGGER;
long V, E;
std::vector<long> *adjacencyList;

// Function to read input graph from file
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

// Brandes' algorithm for betweenness centrality
void brandes()
{
    std::vector<double> CB(V, 0.0); // Centrality array initialized to 0

    for (int s = 0; s < V; s++)
    { // Outer loop over source node `s`
        std::vector<double> delta(V, 0.0);
        std::vector<std::vector<long>> prev(V);
        std::vector<double> sigma(V, 0.0);
        std::vector<double> dist(V, LLONG_MAX);

        sigma[s] = 1; // Initialize source node
        dist[s] = 0;

        std::queue<long> queue;
        queue.push(s);
        std::stack<long> stack;

        // Forward BFS to calculate shortest paths and sigma
        while (!queue.empty())
        {
            long u = queue.front();
            queue.pop();
            stack.push(u);

            for (auto v : adjacencyList[u])
            {
                if (dist[v] == LLONG_MAX)
                { // First visit
                    dist[v] = dist[u] + 1;
                    queue.push(v);
                }

                if (dist[v] == dist[u] + 1)
                { // Shortest path found
                    sigma[v] += sigma[u];
                    prev[v].push_back(u);
                }
            }
        }

        // Backward phase to calculate delta and update CB
        while (!stack.empty())
        {
            long v = stack.top();
            stack.pop();

            for (auto u : prev[v])
            {
                delta[u] += (double(sigma[u]) / sigma[v]) * (1.0 + delta[v]);
            }

            if (v != s)
            { // Update CB[v] with delta
                CB[v] += delta[v];
            }
        }
    }

    // Output raw unnormalized centrality values
    for (int i = 0; i < V; i++)
    {
        std::cout << CB[i] * (1.0 / ((V - 1) * (V - 2))) << " ";
    }
    std::cout << "\n";
}

int main(int argc, char *argv[])
{
    readInput(argv[1]);

    auto start_time = std::chrono::high_resolution_clock::now();
    LOGGER.OUTPUT("The start time is ", getSysTime(start_time));

    brandes();

    auto stop_time = std::chrono::high_resolution_clock::now();
    LOGGER.OUTPUT("The stop time is ", getSysTime(stop_time));

    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
    LOGGER.OUTPUT("Total execution time for single thread: ", time_diff, " milliseconds");

    return 0;
}
