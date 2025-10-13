// main.cpp
#include "common.hpp"
#include <fstream>


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
            if (res.skills[j] > res.skills[j + 1]) {
                swap(res.skills[j], res.skills[j + 1]);
            }
        }
    }
}

void printMatches(const Match m[], int mSize, bool isEmployer, int totalSize) {
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

void matchArray(const Resume arr[], int size, const string userSk[], int userN, Match m[], int& mSize, bool useBin, const string& jobTitle, bool isEmployer) {
    mSize = 0;
    for (int i = 0; i < size; ++i) {
        bool titleMatch = true;
        if (!isEmployer && !jobTitle.empty()) {
            string lowerTitle = arr[i].title;
            for (char& c : lowerTitle) c = tolower(c);
            string lowerJob = jobTitle;
            for (char& c : lowerJob) c = tolower(c);
            if (lowerTitle.find(lowerJob) == string::npos) titleMatch = false;
        }
        if (!titleMatch) continue;
        int matched = 0;
        for (int j = 0; j < userN; ++j) {
            bool found = false;
            if (useBin) {
                found = binarySearchSkill(arr[i].skills, arr[i].numSkills, userSk[j]);
            } else {
                found = linearSearchSkill(arr[i].skills, arr[i].numSkills, userSk[j]);
            }
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
    // Bubble sort matches by perc descending
    for (int i = 0; i < mSize - 1; ++i) {
        for (int j = 0; j < mSize - i - 1; ++j) {
            if (m[j].perc < m[j + 1].perc) swap(m[j], m[j + 1]);
        }
    }
}

int main() {
    cout << "Job Matching System - Array + Merge Sort + Binary Search" << endl;
    
    char role;
    cout << "Are you an Employer (e) looking for candidates or Employee (m) looking for jobs? ";
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
    string skillsPrompt = isEmployer ? "Enter Required Skills for the job (comma-separated): " : "Enter Your Skills (comma-separated): ";
    cout << skillsPrompt;
    string skillsStr;
    getline(cin, skillsStr);

    stringstream ss(skillsStr);
    string token;
    string userSkills[20];
    int userNum = 0;
    while (getline(ss, token, ',')) {
        if (!token.empty()) {
            size_t start = token.find_first_not_of(" \t");
            if (start != string::npos) {
                size_t end = token.find_last_not_of(" \t");
                userSkills[userNum++] = token.substr(start, end - start + 1);
            }
        }
    }

    // Load data into array
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
    cout << "Loaded " << size << " entries" << endl;

    // Merge Sort the array (by numSkills descending)
    auto sortStart = chrono::high_resolution_clock::now();
    if (size > 0) mergeSortArray(arr, 0, size - 1);
    auto sortEnd = chrono::high_resolution_clock::now();
    auto sortDur = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();
    cout << "Sort time: " << sortDur << " ms" << endl;

    // Binary Search and Matching
    auto matchStart = chrono::high_resolution_clock::now();
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    matchArray(arr, size, userSkills, userNum, matches, mSize, true, jobTitle, isEmployer);
    auto matchEnd = chrono::high_resolution_clock::now();
    auto matchDur = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchDur << " ms" << endl;

    printMatches(matches, mSize, isEmployer, size);

    // Write all to file
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
        cout << "All matches saved to matches.txt" << endl;
    } else {
        cout << "Error opening file for writing matches." << endl;
    }

    delete[] arr;
    delete[] matches;
    return 0;
}