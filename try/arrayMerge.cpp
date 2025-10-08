// 1_arrayMerge.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

extern std::string getSkillPart(const std::string&);
extern int getNumSkills(const std::string&);
extern int getMatchCount(const std::string&, const std::string&);

struct Entry {
    std::string desc;
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
        if (L[i].desc <= R[j].desc) {
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

static int binarySearch(Entry arr[], int n, const std::string& target) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid].desc == target) return mid;
        if (arr[mid].desc < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

void arrayMergeMatch(const std::string& title, const std::string& required) {
    // Step 1: Count how many resumes
    std::ifstream countFile("cleaned_resumes.txt");
    if (!countFile.is_open()) {
        std::cout << "Error opening cleaned_resumes.txt" << std::endl;
        return;
    }

    int totalLines = 0;
    std::string tempLine;
    while (std::getline(countFile, tempLine)) {
        if (!tempLine.empty()) ++totalLines;
    }
    countFile.close();

    if (totalLines == 0) {
        std::cout << "No resumes found." << std::endl;
        return;
    }

    // Step 2: Allocate array dynamically for all resumes
    Entry* resumes = new Entry[totalLines];
    std::ifstream file("cleaned_resumes.txt");
    int numResumes = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (std::getline(file, resumes[numResumes].desc)) {
        if (!resumes[numResumes].desc.empty()) ++numResumes;
    }
    file.close();

    auto end = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Loaded " << numResumes << " resumes in " << loadTime.count() << " ms.\n";

    int totalReq = getNumSkills(required);
    if (totalReq == 0) {
        std::cout << "No skills entered." << std::endl;
        delete[] resumes;
        return;
    }

    // Step 3: Find best match score
    double bestScore = 0.0;
    for (int i = 0; i < numResumes; ++i) {
        std::string skillPart = getSkillPart(resumes[i].desc);
        int matchC = getMatchCount(skillPart, required);
        double percent = static_cast<double>(matchC) / totalReq * 100.0;
        if (percent > bestScore) bestScore = percent;
    }

    // Step 4: Print all resumes that achieved the best score
    std::cout << "\nFor job: " << title << std::endl;
    std::cout << "Best match score: " << bestScore << "%\n";
    std::cout << "Top matching candidates:\n";

    int shown = 0;
    for (int i = 0; i < numResumes; ++i) {
        std::string skillPart = getSkillPart(resumes[i].desc);
        int matchC = getMatchCount(skillPart, required);
        double percent = static_cast<double>(matchC) / totalReq * 100.0;

        if (percent == bestScore) {
            std::cout << "[" << (++shown) << "] "
                      << resumes[i].desc.substr(0, 100) << "..." << std::endl;
        }
    }

    if (shown == 0)
        std::cout << "No matches found." << std::endl;

    delete[] resumes;
}