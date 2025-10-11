// array_selection.cpp
#include "common.hpp"

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
    auto loadEnd = chrono::high_resolution_clock::now();
    loadTime = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();
    cout << "Load time: " << loadTime << " ms (" << size << " entries)" << endl;

    auto sortStart = chrono::high_resolution_clock::now();
    if (size > 0) selectionSortArray(arr, size);
    auto sortEnd = chrono::high_resolution_clock::now();
    sortTime = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();
    cout << "Sort time: " << sortTime << " ms" << endl;

    auto matchStart = chrono::high_resolution_clock::now();
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    matchArray(arr, size, userSkills, userNum, matches, mSize, false, jobTitle, isEmployer);
    auto matchEnd = chrono::high_resolution_clock::now();
    matchTime = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchTime << " ms" << endl;

    printMatches(matches, mSize, isEmployer, size);

    delete[] arr;
    delete[] matches;
}