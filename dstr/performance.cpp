#include "performance.hpp"

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
}

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
) 
{
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
    bool useBinary
) 
{
    PerformanceMeasurer measurer;
    const int MAX_SIZE = 10000;
    
    // Measure sort time
    measurer.start();
    head = sortFunction(head);
    sortTime = measurer.end("Sort");
    
    // Measure match time
    measurer.start();
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    matchList(head, userSkills, userNum, matches, mSize, useBinary, jobTitle, isEmployer);
    matchTime = measurer.end("Match");
    
    return PerformanceResult{matches, mSize};
}