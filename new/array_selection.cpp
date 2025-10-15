#include "common.hpp"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <windows.h>  // For memory usage on Windows

using namespace std;
using namespace std::chrono;

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
            for (int i = 1; i < tokCount && res.numSkills < 20; ++i)
                res.skills[res.numSkills++] = tokens[i];
        }
    } else {
        res.title = "Candidate";
        for (int i = 0; i < tokCount && res.numSkills < 20; ++i)
            res.skills[res.numSkills++] = tokens[i];
    }
}

// -------------------- Selection Sort --------------------
void selectionSortArray(Resume arr[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        int maxIndex = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j].numSkills > arr[maxIndex].numSkills)
                maxIndex = j;
        }
        swap(arr[i], arr[maxIndex]);
    }
}

// -------------------- Linear Search --------------------
bool linearSearchSkill(const string sk[20], int n, const string& target) {
    for (int i = 0; i < n; ++i) {
        if (sk[i] == target)
            return true;
    }
    return false;
}

// -------------------- Match Logic --------------------
void matchArray(const Resume arr[], int size, const string userSk[], int userN,
                Match m[], int& mSize, const string& jobTitle, bool isEmployer,
                long long& linearTimeNs, long long& linearCount) {
    mSize = 0;
    linearTimeNs = 0;
    linearCount = 0;

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
            auto s = high_resolution_clock::now();
            bool found = linearSearchSkill(arr[i].skills, arr[i].numSkills, userSk[j]);
            auto e = high_resolution_clock::now();
            linearTimeNs += duration_cast<nanoseconds>(e - s).count();
            ++linearCount;
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

    // Sort matches descending by percentage
    for (int i = 0; i < mSize - 1; ++i)
        for (int j = 0; j < mSize - i - 1; ++j)
            if (m[j].perc < m[j + 1].perc)
                swap(m[j], m[j + 1]);
}



void printMatches(const Match m[], int mSize, bool isEmployer) {
    string header = isEmployer ? "Matching Candidates:" : "Matching Jobs:";
    cout << "\n" << header << endl;
    for (int i = 0; i < mSize && i < 10; ++i) {
        cout << (isEmployer ? "Candidate " : "Job ") << (m[i].id + 1)
             << ": " << fixed << setprecision(2)
             << m[i].perc << "% match — " << m[i].fullDesc << endl;
    }
    if (mSize == 0) cout << "No matches found.\n";
}

// -------------------- MAIN --------------------
int main() {
    auto totalStart = high_resolution_clock::now();
    cout << "========================================\n";
    cout << "   JOB MATCHING SYSTEM (Array + Linear Search + Selection Sort)\n";
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

    cout << (isEmployer ? "\nEnter Required Skills (comma-separated): "
                        : "\nEnter Your Skills (comma-separated): ");
    string skillsStr;
    getline(cin, skillsStr);

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
    cout << "Loading Data...\n";
    cout << "----------------------------------------\n";

    const int MAX_SIZE = 10000;
    Resume* arr = new Resume[MAX_SIZE];
    int size = 0;

    auto loadStart = high_resolution_clock::now();
    ifstream file(filename);
    while (getline(file, token)) {
        if (token.empty()) continue;
        parseSkills(token, arr[size], isJob);
        if (arr[size].numSkills > 0) ++size;
    }
    file.close();
    auto loadEnd = high_resolution_clock::now();
    cout << "Loaded Entries: " << size
         << " | Load Time: " << duration_cast<milliseconds>(loadEnd - loadStart).count() << " ms\n";

    cout << "\nSorting Data (Selection Sort)...\n";
    auto sortStart = high_resolution_clock::now();
    selectionSortArray(arr, size);
    auto sortEnd = high_resolution_clock::now();
    cout << "Sort Completed in " << duration_cast<milliseconds>(sortEnd - sortStart).count() << " ms\n";

    cout << "\nMatching...\n";
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    long long linearTimeNs = 0, linearCount = 0;
    auto matchStart = high_resolution_clock::now();

    matchArray(arr, size, userSkills, userNum, matches, mSize, jobTitle, isEmployer,
               linearTimeNs, linearCount);

    auto matchEnd = high_resolution_clock::now();
    cout << "Match Completed in " << duration_cast<milliseconds>(matchEnd - matchStart).count() << " ms\n";

    if (linearCount > 0) {
        cout << "Linear Search Stats:\n";
        cout << "   - Total Searches: " << linearCount << endl;
        cout << "   - Total Time: " << (linearTimeNs / 1'000'000.0) << " ms\n";
        cout << "   - Avg Time/Search: " << (linearTimeNs / (double)linearCount) << " ns\n";
    }



    printMatches(matches, mSize, isEmployer);

    auto totalEnd = high_resolution_clock::now();
    cout << "\n🏁 TOTAL EXECUTION TIME: "
         << duration_cast<milliseconds>(totalEnd - totalStart).count() << " ms\n";

    delete[] arr;
    delete[] matches;
    return 0;
}