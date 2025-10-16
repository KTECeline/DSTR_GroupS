#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <chrono>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <mach/mach.h>
#endif

using namespace std;

struct Resume {
    string title;
    string skills[20];
    int numSkills;
    string fullDesc;
    Resume* next;
    
    Resume() : numSkills(0), next(nullptr) {}
};

struct Match {
    int id;
    double perc;
    string fullDesc;
};

// Global variables for merge sort analytics
static long long g_mergeCalls = 0;
static long long g_mergeComparisons = 0;
static long long g_mergeCopies = 0;
static long long g_mergeTempBytes = 0;

class LinkedListMatcher {
private:
    Resume* head;
    int size;
    
public:
    LinkedListMatcher() : head(nullptr), size(0) {}
    
    ~LinkedListMatcher() {
        while (head) {
            Resume* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    void addResume(const Resume& res) {
        Resume* newNode = new Resume();
        *newNode = res;
        newNode->next = head;
        head = newNode;
        size++;
    }
    
    int getSize() const { return size; }
    
    Resume* getMiddle(Resume* start) {
        if (!start) return start;
        Resume* slow = start;
        Resume* fast = start;
        Resume* prev = nullptr;
        
        while (fast && fast->next) {
            prev = slow;
            slow = slow->next;
            fast = fast->next->next;
        }
        
        if (prev) prev->next = nullptr;
        return slow;
    }
    
    Resume* merge(Resume* left, Resume* right) {
        if (!left) return right;
        if (!right) return left;
        
        Resume* result = nullptr;
        g_mergeCalls++;
        g_mergeTempBytes += sizeof(Resume*) * 2; // Track pointer operations
        
        g_mergeComparisons++;
        if (left->numSkills >= right->numSkills) {
            result = left;
            result->next = merge(left->next, right);
            g_mergeCopies++;
        } else {
            result = right;
            result->next = merge(left, right->next);
            g_mergeCopies++;
        }
        
        return result;
    }
    
    Resume* mergeSort(Resume* start) {
        if (!start || !start->next) return start;
        
        g_mergeTempBytes += sizeof(Resume*) * 3; // Track local pointers
        
        Resume* middle = getMiddle(start);
        Resume* left = start;
        Resume* right = middle;
        
        left = mergeSort(left);
        right = mergeSort(right);
        
        return merge(left, right);
    }
    
    void sort() {
        head = mergeSort(head);
    }
    
    bool binarySearchSkill(const string skills[], int n, const string& target) {
        int low = 0, high = n - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (skills[mid] == target) return true;
            if (skills[mid] < target) low = mid + 1;
            else high = mid - 1;
        }
        return false;
    }
    
    void matchSkills(const string userSkills[], int userNum, Match matches[], int& mSize, 
                    const string& jobTitle, bool isEmployer, long long& binTimeNs, long long& binCount) {
        mSize = 0;
        binTimeNs = 0;
        binCount = 0;
        
        Resume* current = head;
        int index = 0;
        
        while (current) {
            bool titleMatch = true;
            if (!isEmployer && !jobTitle.empty()) {
                string lowerTitle = current->title;
                string lowerJob = jobTitle;
                for (char& c : lowerTitle) c = tolower(c);
                for (char& c : lowerJob) c = tolower(c);
                if (lowerTitle.find(lowerJob) == string::npos) titleMatch = false;
            }
            
            if (titleMatch) {
                int matched = 0;
                for (int j = 0; j < userNum; ++j) {
                    auto s = chrono::high_resolution_clock::now();
                    bool found = binarySearchSkill(current->skills, current->numSkills, userSkills[j]);
                    auto e = chrono::high_resolution_clock::now();
                    binTimeNs += chrono::duration_cast<chrono::nanoseconds>(e - s).count();
                    ++binCount;
                    if (found) ++matched;
                }
                
                double perc = userNum > 0 ? (static_cast<double>(matched) / userNum) * 100.0 : 0.0;
                if (perc > 0) {
                    matches[mSize].id = index;
                    matches[mSize].perc = perc;
                    matches[mSize].fullDesc = current->fullDesc;
                    ++mSize;
                }
            }
            
            current = current->next;
            index++;
        }
        
        // Sort matches descending by percentage
        for (int i = 0; i < mSize - 1; ++i) {
            for (int j = 0; j < mSize - i - 1; ++j) {
                if (matches[j].perc < matches[j + 1].perc) {
                    Match temp = matches[j];
                    matches[j] = matches[j + 1];
                    matches[j + 1] = temp;
                }
            }
        }
    }
};

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
                string temp = res.skills[j];
                res.skills[j] = res.skills[j + 1];
                res.skills[j + 1] = temp;
            }
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

int main() {
    auto totalStart = chrono::high_resolution_clock::now();
    cout << "========================================\n";
    cout << "   JOB MATCHING SYSTEM (Linked List + Merge Sort + Binary Search)\n";
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

    LinkedListMatcher matcher;
    
    auto loadStart = chrono::high_resolution_clock::now();
    ifstream file(filename);
    bool skipHeader = isJob;
    while (getline(file, token)) {
        if (token.empty()) continue;
        if (skipHeader) { skipHeader = false; continue; }
        
        Resume res;
        parseSkills(token, res, isJob);
        if (res.numSkills > 0) matcher.addResume(res);
    }
    file.close();
    auto loadEnd = chrono::high_resolution_clock::now();
    auto loadDur = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();

    cout << "File: " << filename << endl;
    cout << "Loaded Entries: " << matcher.getSize() << endl;
    cout << "Load Time: " << loadDur << " ms\n";

    cout << "\n----------------------------------------\n";
    cout << "Sorting Data (Merge Sort)\n";
    cout << "----------------------------------------\n";

    auto sortStart = chrono::high_resolution_clock::now();
    matcher.sort();
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

    const int MAX_MATCHES = 10000;
    Match* matches = new Match[MAX_MATCHES];
    int mSize = 0;
    long long binTimeNs = 0, binCount = 0;
    auto matchStart = chrono::high_resolution_clock::now();

    matcher.matchSkills(userSkills, userNum, matches, mSize, jobTitle, isEmployer, binTimeNs, binCount);

    auto matchEnd = chrono::high_resolution_clock::now();
    auto matchDur = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();

    cout << "Match Completed in " << matchDur << " ms\n";
    if (binCount > 0) {
        cout << "Binary Search Stats:\n";
        cout << "   - Total Searches: " << binCount << endl;
        cout << "   - Total Time: " << (binTimeNs / 1'000'000.0) << " ms\n";
        cout << "   - Avg Time/Search: " << (binTimeNs / (double)binCount) << " ns\n";
    }

    // Memory usage
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        cout << "Memory Used (Working Set): " << (pmc.WorkingSetSize / (1024.0 * 1024.0)) << " MB\n";
    }
#else
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) == KERN_SUCCESS) {
        cout << "Memory Used (Resident): " << (info.resident_size / (1024.0 * 1024.0)) << " MB\n";
    }
#endif

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

    delete[] matches;
    return 0;
}