// array_merge.cpp
#include "common.hpp"
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
    auto loadEnd = chrono::high_resolution_clock::now();
    loadTime = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();
    cout << "Load time: " << loadTime << " ms (" << size << " entries)" << endl;

    auto sortStart = chrono::high_resolution_clock::now();
    if (size > 0) mergeSortArray(arr, 0, size - 1);
    auto sortEnd = chrono::high_resolution_clock::now();
    sortTime = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();
    cout << "Sort time: " << sortTime << " ms" << endl;

    auto matchStart = chrono::high_resolution_clock::now();
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    matchArray(arr, size, userSkills, userNum, matches, mSize, true, jobTitle, isEmployer);
    auto matchEnd = chrono::high_resolution_clock::now();
    matchTime = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchTime << " ms" << endl;

    printMatches(matches, mSize, isEmployer, size);

    // Write all to file
    ofstream outFile("matches_array_merge.txt");
    if (outFile.is_open()) {
        string header = isEmployer ? "All Matching Candidates:" : "All Matching Jobs:";
        outFile << header << endl;
        for (int i = 0; i < mSize; ++i) {
            string label = isEmployer ? "Candidate " : "Job ";
            outFile << label << (matches[i].id + 1) << ": " << matches[i].fullDesc << " - " 
                    << fixed << setprecision(2) << matches[i].perc << "% match" << endl;
        }
        if (mSize == 0) outFile << "No matches found." << endl;
        outFile.close();
        cout << "All matches saved to matches_array_merge.txt" << endl;
    } else {
        cout << "Error opening file for writing matches." << endl;
    }

    delete[] arr;
    delete[] matches;
}