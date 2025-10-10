// array_merge.cpp
#include "common.hpp"
#include <fstream>

void mergeSortArray(Resume arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortArray(arr, left, mid);
        mergeSortArray(arr, mid + 1, right);
        // Merge
        int n1 = mid - left + 1;
        int n2 = right - mid;
    // Use heap-allocated temporary storage (manual new[]/delete[]) to avoid large stack allocations
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

void runArrayMerge(const string userSkills[], int userNum, const string& filename, bool isEmployer) {
    bool isJob = !isEmployer;
    cout << "\n--- Array + Merge Sort + Binary Search ---" << endl;

    auto loadStart = chrono::high_resolution_clock::now();
    // Use manual heap array for resumes to avoid std::vector
    Resume* arr = nullptr;
    int capacity = 0;
    int size = 0; // will track actual number of loaded entries
    ifstream file(filename);
    string line;
    bool skipHeader = isJob;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (skipHeader) {
            skipHeader = false;
            continue;
        }
        if (size >= 10000) break;
        Resume tmp;
        parseSkills(line, tmp, isJob);
        if (tmp.numSkills > 0) {
            if (size >= capacity) {
                int newCap = (capacity == 0) ? 256 : capacity * 2;
                if (newCap > 10000) newCap = 10000;
                Resume* newArr = new Resume[newCap];
                for (int i = 0; i < size; ++i) newArr[i] = arr[i];
                delete[] arr;
                arr = newArr;
                capacity = newCap;
            }
            arr[size++] = tmp;
        }
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
    // Create a temporary buffer on the heap for matches and call matchArray
    Match* tmpMatches = new Match[10000];
    int mSize = 0;
    matchArray(arr, size, userSkills, userNum, tmpMatches, mSize, true);
    auto matchEnd = chrono::high_resolution_clock::now();
    auto matchDur = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchDur << " ms" << endl;

    // printMatches expects an array; pass pointer to first element of vector if non-empty
    if (mSize > 0) printMatches(tmpMatches, mSize, isEmployer, size);
    else printMatches(nullptr, 0, isEmployer, size);

    // Write all to file
    ofstream outFile("matches_array_merge.txt");
    if (outFile.is_open()) {
        string header = isEmployer ? "All Matching Candidates:" : "All Matching Jobs:";
        outFile << header << endl;
    for (int i = 0; i < mSize; ++i) {
        string label = isEmployer ? "Candidate " : "Job ";
        outFile << label << (tmpMatches[i].id + 1) << ": " << tmpMatches[i].fullDesc << " - " 
            << fixed << setprecision(2) << tmpMatches[i].perc << "% match" << endl;
    }
        if (mSize == 0) outFile << "No matches found." << endl;
        outFile.close();
        cout << "All matches saved to matches_array_merge.txt" << endl;
    } else {
        cout << "Error opening file for writing matches." << endl;
    }
    delete[] tmpMatches;
    delete[] arr;
}