#ifndef PERFORMANCE_HPP
#define PERFORMANCE_HPP

#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "common.hpp"

struct Node;  // Forward declaration for Node

void writeMatchesToFile(const Match* matches, int mSize, bool isEmployer, const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        std::string header = isEmployer ? "All Matching Candidates:" : "All Matching Jobs:";
        outFile << header << std::endl;
        for (int i = 0; i < mSize; ++i) {
            std::string label = isEmployer ? "Candidate " : "Job ";
            outFile << label << (matches[i].id + 1) << ": " << matches[i].fullDesc << " - " 
                    << std::fixed << std::setprecision(2) << matches[i].perc << "% match" << std::endl;
        }
        if (mSize == 0) outFile << "No matches found." << std::endl;
        outFile.close();
        std::cout << "All matches saved to " << filename << std::endl;
    } else {
        std::cout << "Error opening file for writing matches." << std::endl;
    }

class PerformanceMeasurer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;

public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    double end(const std::string& operationName, int size = -1) {
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
) {
    PerformanceMeasurer measurer;
    const int MAX_SIZE = 10000;
    
    // Measure sort time
    measurer.start();
    if (size > 0) sortFunction(arr, size);
    sortTime = measurer.end("Sort");
    
    // Measure match time
    measurer.start();
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    matchArray(arr, size, userSkills, userNum, matches, mSize, false, jobTitle, isEmployer);
    matchTime = measurer.end("Match");
    
    return PerformanceResult{matches, mSize};
}

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
) {
    PerformanceMeasurer measurer;
    const int MAX_SIZE = 10000;
    
    // Measure sort time
    measurer.start();
    if (head) head = sortFunction(head);
    sortTime = measurer.end("Sort");
    
    // Measure match time
    measurer.start();
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    matchList(head, userSkills, userNum, matches, mSize, useBinarySearch, jobTitle, isEmployer);
    matchTime = measurer.end("Match");
    
    return PerformanceResult{matches, mSize};
}

#endif // PERFORMANCE_HPP