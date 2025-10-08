// 2_arrayBinary.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

extern std::string getSkillPart(const std::string&);
extern int getNumSkills(const std::string&);
extern int getMatchCount(const std::string&, const std::string&);

struct Entry {
    std::string desc;
    double score;
};

static void merge(Entry arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    Entry* L = new Entry[n1];
    Entry* R = new Entry[n2];
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int i = 0; i < n2; ++i) R[i] = arr[mid + 1 + i];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].score >= R[j].score) {  // Descending
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

static void mergeSort(Entry arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

static int findFirstLowScore(Entry arr[], int n) {
    int low = 0, high = n - 1;
    int result = n;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid].score <= 0.0) {
            result = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return result;
}

void arrayBinaryMatch(const std::string& title, const std::string& required) {
    const int MAX = 500;
    Entry resumes[MAX];
    int numResumes = 0;

    std::ifstream file("cleaned_resumes.txt");
    if (!file.is_open()) {
        std::cout << "Error opening cleaned_resumes.txt" << std::endl;
        return;
    }
    std::string line;
    auto start = std::chrono::high_resolution_clock::now();
    while (std::getline(file, line) && numResumes < MAX) {
        if (!line.empty()) {
            resumes[numResumes].desc = line;
            resumes[numResumes].score = 0.0;
            ++numResumes;
        }
    }
    file.close();
    auto end = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Load time: " << loadTime.count() << " ms" << std::endl;

    int totalReq = getNumSkills(required);
    if (totalReq == 0) {
        std::cout << "No skills entered." << std::endl;
        return;
    }

    // Compute scores
    for (int i = 0; i < numResumes; ++i) {
        std::string skillPart = getSkillPart(resumes[i].desc);
        int matchC = getMatchCount(skillPart, required);
        resumes[i].score = static_cast<double>(matchC) / totalReq * 100.0;
    }

    start = std::chrono::high_resolution_clock::now();
    mergeSort(resumes, 0, numResumes - 1);
    end = std::chrono::high_resolution_clock::now();
    auto sortTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Sort time (by score descending): " << sortTime.count() << " ms" << std::endl;

    // Use binary search to find cutoff
    int cutoff = findFirstLowScore(resumes, numResumes);
    std::cout << "For job: " << title << std::endl;
    std::cout << "Matched candidates (using binary search cutoff):" << std::endl;
    for (int i = 0; i < cutoff; ++i) {
        std::cout << "Resume: " << resumes[i].desc.substr(0, 100) << "... Match: " << resumes[i].score << "%" << std::endl;
    }
    if (cutoff == 0) std::cout << "No matches found." << std::endl;
}