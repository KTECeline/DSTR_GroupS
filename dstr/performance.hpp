#ifndef PERFORMANCE_HPP
#define PERFORMANCE_HPP

#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "common.hpp"

struct Node;  // Forward declaration for Node

void writeMatchesToFile(const Match* matches, int mSize, bool isEmployer, const std::string& filename);

class PerformanceMeasurer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;

public:
    inline void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    inline double end(const std::string& operationName, int size = -1) {
        endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        if (size >= 0) {
            std::cout << operationName << " time: " << duration << " ms (" << size << " entries)" << std::endl;
        } else {
            std::cout << operationName << " time: " << duration << " ms" << std::endl;
        }
        
        return duration;
    }
};

struct PerformanceResult {
    Match* matches;
    int mSize;
};

PerformanceResult measureAndExecuteOperations(
    Resume arr[],
    int size,
    const std::string userSkills[],
    int userNum,
    bool isEmployer,
    const std::string& jobTitle,
    void (*sortFunction)(Resume[], int),
    double& loadTime,
    double& sortTime,
    double& matchTime
);

PerformanceResult measureAndExecuteOperationsList(
    Node*& head,
    int size,
    const std::string userSkills[],
    int userNum,
    bool isEmployer,
    const std::string& jobTitle,
    Node* (*sortFunction)(Node*),
    double& loadTime,
    double& sortTime,
    double& matchTime,
    bool useBinarySearch
);

#endif // PERFORMANCE_HPP