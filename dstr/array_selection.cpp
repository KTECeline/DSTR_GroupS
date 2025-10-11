// array_selection.cpp
#include "common.hpp"
#include "performance.hpp"

void selectionSortArray(Resume arr[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        int maxIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j].numSkills > arr[maxIdx].numSkills) maxIdx = j;
        }
        swap(arr[i], arr[maxIdx]);
    }
}

void runArraySelection(const string userSkills[], int userNum, const string& filename, bool isEmployer, const string& jobTitle, double& loadTime, double& sortTime, double& matchTime) {
    bool isJob = !isEmployer;
    cout << "\n--- Array + Selection Sort + Linear Search ---" << endl;

    auto loadStart = chrono::high_resolution_clock::now();
    const int MAX_SIZE = 10000;
    Resume* arr = new Resume[MAX_SIZE];
    int size = 0;
    ifstream file(filename);
    string line;
    bool skipHeader = isJob;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (skipHeader) {
            skipHeader = false;
            continue;
        }
        if (size >= MAX_SIZE) break;
        parseSkills(line, arr[size], isJob);
        if (arr[size].numSkills > 0) ++size;
    }
    file.close();
    PerformanceMeasurer measurer;
    loadTime = measurer.end("Load", size);
    
    // Use the function to handle sort and match operations
    auto result = measureAndExecuteOperations(
        arr,
        size,
        userSkills,
        userNum,
        isEmployer,
        jobTitle,
        selectionSortArray,
        loadTime,
        sortTime,
        matchTime
    );
    
    Match* matches = result.matches;
    int mSize = result.mSize;
    
    printMatches(matches, mSize, isEmployer, size);
    writeMatchesToFile(matches, mSize, isEmployer, "matches_array_selection.txt");

    delete[] arr;
    delete[] matches;
}