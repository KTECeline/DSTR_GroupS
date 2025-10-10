// array_merge.cpp
#include "common.hpp"

void mergeSortArray(Resume arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortArray(arr, left, mid);
        mergeSortArray(arr, mid + 1, right);
        // Merge
        int n1 = mid - left + 1;
        int n2 = right - mid;
        Resume L[100], R[100];
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
    }
}

void runArrayMerge(const string userSkills[], int userNum, const string& filename, bool isEmployer) {
    bool isJob = !isEmployer;
    cout << "\n--- Array + Merge Sort + Binary Search ---" << endl;

    auto loadStart = chrono::high_resolution_clock::now();
    Resume arr[100];
    int size = 0;
    ifstream file(filename);
    string line;
    bool skipHeader = isJob;
    while (getline(file, line) && size < 100) {
        if (line.empty()) continue;
        if (skipHeader) {
            skipHeader = false;
            continue;
        }
        parseSkills(line, arr[size], isJob);
        if (arr[size].numSkills > 0) ++size;
    }
    file.close();
    auto loadEnd = chrono::high_resolution_clock::now();
    auto loadDur = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();
    cout << "Load time: " << loadDur << " ms (" << size << " entries)" << endl;

    auto sortStart = chrono::high_resolution_clock::now();
    if (size > 0) mergeSortArray(arr, 0, size - 1);
    auto sortEnd = chrono::high_resolution_clock::now();
    auto sortDur = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();
    cout << "Sort time (Merge Sort): " << sortDur << " ms" << endl;

    auto matchStart = chrono::high_resolution_clock::now();
    Match matches[100];
    int mSize = 0;
    matchArray(arr, size, userSkills, userNum, matches, mSize, true);
    auto matchEnd = chrono::high_resolution_clock::now();
    auto matchDur = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchDur << " ms" << endl;

    printMatches(matches, mSize, isEmployer);
}