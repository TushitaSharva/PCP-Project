#include <iostream>
#include <vector>
#include <random>
#include <string.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <mutex>
#include <time.h>
#include <chrono>
#include <fstream>
#include <atomic>
#include <mutex>
#include <thread>

// Logger class, for printing and logging to log files and debugging
class Logger
{
public:
    template<typename... Args>
    void DEBUG(Args... args)
    {
        std::ofstream debugfile("outputs/debug.txt", std::ios::app);
        std::ostringstream oss;
        (oss << ... << args);  // Use fold expression to stream all arguments
        debugfile << oss.str() << "\n";
        debugfile.close();
    }

    template<typename... Args>
    void OUTPUT(Args... args)
    {
        std::ofstream outputfile("outputs/out.txt", std::ios::app);
        std::ostringstream oss;
        (oss << ... << args);  // Stream all arguments
        outputfile << oss.str() << "\n";
        outputfile.close();
    }
};

// Function to convert time_point to a formatted string
std::string getSysTime(const std::chrono::high_resolution_clock::time_point& tp)
{
    auto time_t = std::chrono::high_resolution_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&time_t); // Convert to local time
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", tm);
    return std::string(buffer);
}

static Logger LOGGER;
long n;
std::vector<std::vector<long>> adjacencyList(n); 

void readInput(std::string filename)
{
    std::ifstream inputfile(filename);

    inputfile >> n;
    std::string str;
    int number;
    std::getline(inputfile, str); // to remove the first line

    for(int i = 0; i < n; i++)
    {
        std::vector<long> neighbours;
        std::getline(inputfile, str);
        std::stringstream iss(str);

        while(iss >> number)
        {
            neighbours.push_back(number);
        }

        adjacencyList.push_back(neighbours);
    }

    inputfile.close();
    return;
}

int main(int argc, char *argv[])
{
    readInput(argv[0]);
    auto start_time = std::chrono::high_resolution_clock::now();
    LOGGER.OUTPUT("The start time is ", getSysTime(start_time));

    // Your code goes here

    auto stop_time = std::chrono::high_resolution_clock::now();
    LOGGER.OUTPUT("The stop time is ", getSysTime(stop_time));

    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
    LOGGER.OUTPUT("Total execution time: ", time_diff, " milliseconds");

    return 0;
}