#include <iostream>
#include <vector>
#include <random>
#include <string.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <sstream>
#include <mutex>
#include <time.h>
#include <chrono>
#include <fstream>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <stack>

// Logger class, for printing and logging to log files and debugging
class Logger
{
public:
    template <typename... Args>
    void DEBUG(Args... args)
    {
        std::ofstream debugfile("outputs/debug.txt", std::ios::app);
        std::ostringstream oss;
        (oss << ... << args); // Use fold expression to stream all arguments
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
    return;
}

std::vector<double> CB;
std::mutex *arrMutexes;
void readInput()
{
    V = 10; E = 18;
    adjacencyList = new std::vector<long>[V];
    adjacencyList[0].push_back(1);
    adjacencyList[0].push_back(2);
    adjacencyList[1].push_back(0);
    adjacencyList[1].push_back(3);
    adjacencyList[1].push_back(4);
    adjacencyList[2].push_back(0);
    adjacencyList[2].push_back(5);
    adjacencyList[2].push_back(6);
    adjacencyList[3].push_back(1);
    adjacencyList[3].push_back(7);
    adjacencyList[4].push_back(1);
    adjacencyList[4].push_back(8);
    adjacencyList[4].push_back(9);
    adjacencyList[5].push_back(2);
    adjacencyList[6].push_back(2);
    adjacencyList[7].push_back(3);
    adjacencyList[8].push_back(4);
    adjacencyList[9].push_back(4);
}


void brandes(int s)
{
    for (int v = 0; v < V; v++) // inner loop
    {
        LOGGER.DEBUG("s, v: ", s, " ", v);
        std::vector<double> delta(V, 0.0);      // Number of shortest paths from s to v
        std::vector<std::vector<long>> prev(V); // Immediate predecessors of v during BFS
        std::vector<long> sigma(V, 0);          // Number of shortest paths from s to v
        std::vector<long> dist(V, LLONG_MAX);

        sigma[s] = 1;
        dist[s] = 0;

        std::queue<long> queue;
        queue.push(s);
        std::stack<long> stack;

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
                    sigma[v] = sigma[v] + sigma[u];
                    prev[v].push_back(u);
                }
            }
        }

        while (!stack.empty())
        {
            int v = stack.top();
            stack.pop();
            for (auto u : prev[v])
            {
                delta[u] += (sigma[u] / sigma[v]) * (1.0 + delta[v]);

                if (v != s)
                {
                    arrMutexes[v].lock();
                    CB[v] += delta[v];
                    arrMutexes[v].unlock();
                }
            }
        }

        std::string str = "";
        for (int i = 0; i < V; i++)
        {
            str += std::to_string(CB[i]) + " ";
        }

        LOGGER.DEBUG(str);
        LOGGER.DEBUG("---------------------");
    }

    LOGGER.DEBUG("=========================================");
}

int main(int argc, char *argv[])
{
    readInput(argv[1]);
    CB.resize(V, 0.0); // Initialize CB after V is known
    arrMutexes = new std::mutex[V];

    auto start_time = std::chrono::high_resolution_clock::now();
    LOGGER.OUTPUT("The start time is ", getSysTime(start_time));

    std::thread threads[V];

    for (int s = 0; s < V; s++)
    {
        threads[s] = std::thread(brandes, s);
    }

    for(auto& th: threads)
    {
        th.join();
    }

    for (int i = 0; i < V; i++)
    {
        std::cout << CB[i] << " ";
    }
    std::cout << "\n";

    auto stop_time = std::chrono::high_resolution_clock::now();
    LOGGER.OUTPUT("The stop time is ", getSysTime(stop_time));

    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
    LOGGER.OUTPUT("Total execution time for single loop parallelized: ", time_diff, " milliseconds");

    return 0;
}