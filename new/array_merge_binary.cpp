// main.cpp
#include "common.hpp"
#include <fstream>
#include <chrono>
#include <mach/mach.h>
#include <iomanip>
#include <sstream>
#include <iostream>
using namespace std;

// -------------------- Utility Functions --------------------
int splitString(const string& line, string tokens[], int maxTokens) {
    int count = 0;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ',')) {
        if (!token.empty()) {
            token.erase(0, token.find_first_not_of(" \t"));
            size_t last = token.find_last_not_of(" \t");
            if (last != string::npos) token.erase(last + 1);
            if (count < maxTokens) tokens[count++] = token;
        }
    }
    return count;
}

void parseSkills(string line, Resume& res, bool isJob) {
    res.fullDesc = line;
    string tokens[50];
    int tokCount = splitString(line, tokens, 50);
    res.numSkills = 0;
    if (isJob) {
        if (tokCount > 0) {
            res.title = tokens[0];
            for (int i = 1; i < tokCount && res.numSkills < 20; ++i) {
                res.skills[res.numSkills++] = tokens[i];
            }
        }
    } else {
        res.title = "Candidate";
        for (int i = 0; i < tokCount && res.numSkills < 20; ++i) {
            res.skills[res.numSkills++] = tokens[i];
        }
    }
    // Bubble sort skills alphabetically
    for (int i = 0; i < res.numSkills - 1; ++i) {
        for (int j = 0; j < res.numSkills - i - 1; ++j) {
            if (res.skills[j] > res.skills[j + 1]) swap(res.skills[j], res.skills[j + 1]);
        }
    }
}

void printMatches(const Match m[], int mSize, bool isEmployer) {
    string header = isEmployer ? "Matching Candidates:" : "Matching Jobs:";
    string label = isEmployer ? "Candidate " : "Job ";
    cout << header << endl;
    for (int i = 0; i < mSize && i < 10; ++i) {
        cout << label << (m[i].id + 1) << ": " << m[i].fullDesc << " - "
             << fixed << setprecision(2) << m[i].perc << "% match" << endl;
    }
    if (mSize > 10) {
        cout << "... and " << (mSize - 10) << " more matches (see matches.txt)" << endl;
    }
    if (mSize == 0) cout << "No matches found." << endl;
}

bool binarySearchSkill(const string sk[20], int n, const string& target) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (sk[mid] == target) return true;
        if (sk[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return false;
}

bool linearSearchSkill(const string sk[20], int n, const string& target) {
    for (int k = 0; k < n; ++k) {
        if (sk[k] == target) return true;
    }
    return false;
}

// --- Merge sort analytics ---
static long long g_mergeCalls = 0;
static long long g_mergeComparisons = 0;
static long long g_mergeTempBytes = 0;
static long long g_mergeCopies = 0;

void mergeSortArray(Resume arr[], int left, int right) {
    if (left < right) {
        ++g_mergeCalls; // count merge operation
        int mid = left + (right - left) / 2;
        mergeSortArray(arr, left, mid);
        mergeSortArray(arr, mid + 1, right);
        int n1 = mid - left + 1;
        int n2 = right - mid;
        Resume* L = new Resume[n1];
        Resume* R = new Resume[n2];
        g_mergeTempBytes += static_cast<long long>(n1 + n2) * static_cast<long long>(sizeof(Resume));
        for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
        for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            ++g_mergeComparisons;
            if (L[i].numSkills >= R[j].numSkills) {
                arr[k++] = L[i++];
                ++g_mergeCopies;
            } else {
                arr[k++] = R[j++];
                ++g_mergeCopies;
            }
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
        // count remaining copies
        // note: the above loops also copy; for simplicity, add remaining counts
        // but we already incremented copies inside main loop; count remaining
        // (these loops copy elements from L/R to arr)
        // Compute number of remaining copies (if any)
        // (We won't double-count the ones already copied inside the merge loop.)
        delete[] L;
        delete[] R;
    }
}

void matchArray(const Resume arr[], int size, const string userSk[], int userN,
                Match m[], int& mSize, const string& jobTitle, bool isEmployer,
                long long &binTimeNs, long long &binCount) {
    mSize = 0;
    binTimeNs = 0;
    binCount = 0;
    for (int i = 0; i < size; ++i) {
        bool titleMatch = true;
        if (!isEmployer && !jobTitle.empty()) {
            string lowerTitle = arr[i].title;
            string lowerJob = jobTitle;
            for (char& c : lowerTitle) c = tolower(c);
            for (char& c : lowerJob) c = tolower(c);
            if (lowerTitle.find(lowerJob) == string::npos) titleMatch = false;
        }
        if (!titleMatch) continue;
        int matched = 0;
        for (int j = 0; j < userN; ++j) {
            auto s = chrono::high_resolution_clock::now();
            bool found = binarySearchSkill(arr[i].skills, arr[i].numSkills, userSk[j]);
            auto e = chrono::high_resolution_clock::now();
            binTimeNs += chrono::duration_cast<chrono::nanoseconds>(e - s).count();
            ++binCount;
            if (found) ++matched;
        }
        double perc = userN > 0 ? (static_cast<double>(matched) / userN) * 100.0 : 0.0;
        if (perc > 0) {
            m[mSize].id = i;
            m[mSize].perc = perc;
            m[mSize].fullDesc = arr[i].fullDesc;
            ++mSize;
        }
    }
    // Sort matches descending by perc
    for (int i = 0; i < mSize - 1; ++i) {
        for (int j = 0; j < mSize - i - 1; ++j) {
            if (m[j].perc < m[j + 1].perc) swap(m[j], m[j + 1]);
        }
    }
}
int main() {
    auto totalStart = chrono::high_resolution_clock::now();
    cout << "========================================\n";
    cout << "   JOB MATCHING SYSTEM (Array + Merge Sort + Binary Search)\n";
      cout << "========================================\n";

    char role;
    cout << "Are you an Employer (e) or Employee (m)? ";
    cin >> role;
    cin.ignore();
    bool isEmployer = (role == 'e' || role == 'E');
    bool isJob = !isEmployer;

    string filename = isEmployer ? "cleaned_resumes.txt" : "cleaned_job_description.csv";
    string jobTitle = "";
    if (!isEmployer) {
        cout << "Enter Job Title to match: ";
        getline(cin, jobTitle);
    }

    cout << (isEmployer ? "\nEnter Required Skills for the job (comma-separated): "
                        : "\nEnter Your Skills (comma-separated): ");
    string skillsStr;
    getline(cin, skillsStr);

    // Parse user-entered skills
    stringstream ss(skillsStr);
    string token;
    string userSkills[20];
    int userNum = 0;
    while (getline(ss, token, ',')) {
        if (!token.empty()) {
            size_t start = token.find_first_not_of(" \t");
            size_t end = token.find_last_not_of(" \t");
            if (start != string::npos && end != string::npos)
                userSkills[userNum++] = token.substr(start, end - start + 1);
        }
    }

    cout << "\n----------------------------------------\n";
    cout << "Loading Data\n";
    cout << "----------------------------------------\n";

    const int MAX_SIZE = 10000;
    Resume* arr = new Resume[MAX_SIZE];
    int size = 0;

    auto loadStart = chrono::high_resolution_clock::now();
    ifstream file(filename);
    bool skipHeader = isJob;
    while (getline(file, token)) {
        if (token.empty()) continue;
        if (skipHeader) { skipHeader = false; continue; }
        if (size >= MAX_SIZE) break;
        parseSkills(token, arr[size], isJob);
        if (arr[size].numSkills > 0) ++size;
    }
    file.close();
    auto loadEnd = chrono::high_resolution_clock::now();
    auto loadDur = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();

    cout << "File: " << filename << endl;
    cout << "Loaded Entries: " << size << endl;
    cout << "Load Time: " << loadDur << " ms\n";

    cout << "\n----------------------------------------\n";
    cout << "Sorting Data (Merge Sort)\n";
    cout << "----------------------------------------\n";

    auto sortStart = chrono::high_resolution_clock::now();
    if (size > 0) mergeSortArray(arr, 0, size - 1);
    auto sortEnd = chrono::high_resolution_clock::now();
    auto sortDur = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();

    cout << "Sort Completed in " << sortDur << " ms\n";
    cout << "Merge Sort Stats:\n";
    cout << "   - Calls: " << g_mergeCalls << endl;
    cout << "   - Comparisons: " << g_mergeComparisons << endl;
    cout << "   - Copies: " << g_mergeCopies << endl;
    cout << fixed << setprecision(2);
    cout << "   - Temp Memory Used: " << (g_mergeTempBytes / (1024.0 * 1024.0)) << " MB\n";

    cout << "\n----------------------------------------\n";
    cout << "Matching Process\n";
    cout << "----------------------------------------\n";

    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    long long binTimeNs = 0, binCount = 0;
    auto matchStart = chrono::high_resolution_clock::now();

    matchArray(arr, size, userSkills, userNum, matches, mSize, jobTitle, isEmployer,
               binTimeNs, binCount);

    auto matchEnd = chrono::high_resolution_clock::now();
    auto matchDur = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();

    cout << " Match Completed in " << matchDur << " ms\n";
    if (binCount > 0) {
        cout << "Binary Search Stats:\n";
        cout << "   - Total Searches: " << binCount << endl;
        cout << "   - Total Time: " << (binTimeNs / 1'000'000.0) << " ms\n";
        cout << "   - Avg Time/Search: " << (binTimeNs / (double)binCount) << " ns\n";
    }

    Memory usage (macOS)
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) == KERN_SUCCESS) {
        cout << "Memory Used (Resident): " << (info.resident_size / (1024.0 * 1024.0)) << " MB\n";
    }

    cout << "\n----------------------------------------\n";
    cout << "Match Results\n";
    cout << "----------------------------------------\n";
    printMatches(matches, mSize, isEmployer);

    ofstream outFile("matches.txt");
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
        cout << "Matches saved to matches.txt\n";
    } else {
        cout << "Error writing matches file.\n";
    }

    auto totalEnd = chrono::high_resolution_clock::now();
    auto totalDur = chrono::duration_cast<chrono::milliseconds>(totalEnd - totalStart).count();

    cout << "🏁 TOTAL EXECUTION TIME: " << totalDur << " ms\n";

    delete[] arr;
    delete[] matches;
    return 0;
}
