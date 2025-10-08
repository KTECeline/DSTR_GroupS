// 1_arrayMerge.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <cctype>

extern std::string normalize(std::string);
extern std::string getSkillPart(const std::string&);
extern int getNumSkills(const std::string&);
extern int getMatchCount(const std::string&, const std::string&);

struct Entry {
    std::string desc;
};

struct Job {
    std::string title;
    std::string skills;
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

void arrayMergeMatch() {
    const int MAX = 500;
    const int MAXJ = 100;
    Entry resumes[MAX];
    Job jobs[MAXJ];
    int numResumes = 0;
    int numJobs = 0;

    // Load jobs
    std::ifstream jfile("cleaned_job_description.csv");
    if (jfile.is_open()) {
        std::string jline;
        std::getline(jfile, jline);  // Skip header
        while (std::getline(jfile, jline) && numJobs < MAXJ) {
            if (!jline.empty()) {
                size_t comma = jline.find(',');
                if (comma != std::string::npos) {
                    jobs[numJobs].title = jline.substr(0, comma);
                    jobs[numJobs].skills = jline.substr(comma + 1);
                    ++numJobs;
                }
            }
        }
        jfile.close();
    }

    // Load resumes
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
            ++numResumes;
        }
    }
    file.close();
    auto end = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Load time: " << loadTime.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    mergeSort(resumes, 0, numResumes - 1);
    end = std::chrono::high_resolution_clock::now();
    auto sortTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Sort time: " << sortTime.count() << " ms" << std::endl;

    // Demonstrate binary search
    if (numResumes > 0) {
        int idx = binarySearch(resumes, numResumes, resumes[0].desc);
        std::cout << "Binary search demo: Found first resume at index " << idx << std::endl;
    }

    std::string title, required;
    std::cout << "Enter job title: ";
    std::getline(std::cin, title);
    std::string norm_title = normalize(title);
    bool found = false;
    for (int i = 0; i < numJobs; ++i) {
        std::string norm_jtitle = normalize(jobs[i].title);
        if (norm_jtitle == norm_title) {
            required = jobs[i].skills;
            found = true;
            std::cout << "Found job, using skills: " << required << std::endl;
            break;
        }
    }
    if (!found) {
        std::cout << "Job not found, enter required skills (comma-separated): ";
        std::getline(std::cin, required);
    }

    int totalReq = getNumSkills(required);
    if (totalReq == 0) {
        std::cout << "No skills entered." << std::endl;
        return;
    }

    std::cout << "For job: " << title << std::endl;
    std::cout << "Matched candidates:" << std::endl;
    int matchedCount = 0;
    for (int i = 0; i < numResumes; ++i) {
        std::string skillPart = getSkillPart(resumes[i].desc);
        int matchC = getMatchCount(skillPart, required);
        double percent = static_cast<double>(matchC) / totalReq * 100.0;
        if (percent > 0.0) {
            std::cout << "Resume: " << resumes[i].desc.substr(0, 100) << "... Match: " << percent << "%" << std::endl;
            ++matchedCount;
        }
    }
    if (matchedCount == 0) std::cout << "No matches found." << std::endl;
}