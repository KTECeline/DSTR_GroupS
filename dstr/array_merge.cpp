// array_merge.cpp
#include "common.hpp"
#include "performance.hpp"
#include <fstream>

void mergeSortArray(Resume arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortArray(arr, left, mid);
        mergeSortArray(arr, mid + 1, right);
        int n1 = mid - left + 1;
        int n2 = right - mid;
        Resume* L = new Resume[n1];
        Resume* R = new Resume[n2];
        for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
        for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i].numSkills >= R[j].numSkills) {
                arr[k++] = L[i++];
            } else {
                arr[k++] = R[j++];
            }
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
        delete[] L;
        delete[] R;
    }
}

void runArrayMerge(const string userSkills[], int userNum, const string& filename, bool isEmployer, const string& jobTitle, double& loadTime, double& sortTime, double& matchTime) {
    bool isJob = !isEmployer;
    cout << "\n--- Array + Merge Sort + Binary Search ---" << endl;

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

    auto mergeSortWrapper = [](Resume arr[], int size) {
        if (size > 0) mergeSortArray(arr, 0, size - 1);
    };
    
    auto result = measureAndExecuteOperations(
        arr,
        size,
        userSkills,
        userNum,
        isEmployer,
        jobTitle,
        mergeSortWrapper,
        loadTime,
        sortTime,
        matchTime
    );
    
    Match* matches = result.matches;
    int mSize = result.mSize;
    
    printMatches(matches, mSize, isEmployer, size);
    writeMatchesToFile(matches, mSize, isEmployer, "matches_array_merge.txt");

    delete[] arr;
    delete[] matches;
}