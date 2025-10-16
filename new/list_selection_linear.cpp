// main.cpp
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

#include "common.hpp"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
using namespace std;
// #include <mach/mach.h>  // macOS memory tracking

// -------------------- Linked List Node --------------------
struct Node {
    Resume data;    // Contains: title, skills[20], numSkills, fullDesc
    Node* next;     // Single direction traversal (forward only)
    
    Node(const Resume& r) : data(r), next(nullptr) {}
};

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
    // Note: No sorting of skills for linear search
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

// Simple linear search for skill in skills array
// O(n) complexity

// Unsorted array, so linear search is suitable; Small search space; Simple implementation
bool linearSearchSkill(const string sk[20], int n, const string& target) {
    for (int k = 0; k < n; ++k) {
        if (sk[k] == target) return true;
    }
    return false;
}

// -------------------- Linked List Operations --------------------
void insertAtEnd(Node*& head, const Resume& r) {
    Node* newNode = new Node(r);
    if (head == nullptr) {
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = newNode;
}

int getListSize(Node* head) {
    int count = 0;
    Node* temp = head;
    while (temp != nullptr) {
        ++count;
        temp = temp->next;
    }
    return count;
}

void deleteList(Node*& head) {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

// --- Selection Sort analytics ---
static long long g_selectionComparisons = 0;
static long long g_selectionSwaps = 0;

// Simple implementation of selection sort on linked list
// Minimises number of swaps

// May be inefficient for large lists due to O(n^2) complexity
void selectionSortList(Node* head) {
    if (head == nullptr) return;
    
    Node* current = head;
    while (current != nullptr) {
        Node* maxNode = current;
        Node* temp = current->next;
        
        // Find the node with maximum numSkills in remaining list
        while (temp != nullptr) {
            ++g_selectionComparisons;
            if (temp->data.numSkills > maxNode->data.numSkills) {
                maxNode = temp;
            }
            temp = temp->next;
        }
        
        // Swap data if needed
        if (maxNode != current) {
            Resume tempData = current->data;
            current->data = maxNode->data;
            maxNode->data = tempData;
            ++g_selectionSwaps;
        }
        
        current = current->next;
    }
}

void matchLinkedList(Node* head, const string userSk[], int userN,
                     Match m[], int& mSize, const string& jobTitle, bool isEmployer,
                     long long &linTimeNs, long long &linCount) {
    mSize = 0;
    linTimeNs = 0;
    linCount = 0;
    
    Node* current = head;
    int id = 0;
    
    while (current != nullptr) {
        bool titleMatch = true;
        if (!isEmployer && !jobTitle.empty()) {
            string lowerTitle = current->data.title;
            string lowerJob = jobTitle;
            for (char& c : lowerTitle) c = tolower(c);
            for (char& c : lowerJob) c = tolower(c);
            if (lowerTitle.find(lowerJob) == string::npos) titleMatch = false;
        }
        
        if (titleMatch) {
            int matched = 0;
            for (int j = 0; j < userN; ++j) {
                auto s = chrono::high_resolution_clock::now();
                bool found = linearSearchSkill(current->data.skills, current->data.numSkills, userSk[j]);
                auto e = chrono::high_resolution_clock::now();
                linTimeNs += chrono::duration_cast<chrono::nanoseconds>(e - s).count();
                ++linCount;
                if (found) ++matched;
            }
            
            double perc = userN > 0 ? (static_cast<double>(matched) / userN) * 100.0 : 0.0;
            if (perc > 0) {
                m[mSize].id = id;
                m[mSize].perc = perc;
                m[mSize].fullDesc = current->data.fullDesc;
                ++mSize;
            }
        }
        
        current = current->next;
        ++id;
    }
    
    // Sort matches descending by perc using bubble sort
    for (int i = 0; i < mSize - 1; ++i) {
        for (int j = 0; j < mSize - i - 1; ++j) {
            if (m[j].perc < m[j + 1].perc) swap(m[j], m[j + 1]);
        }
    }
}

int main() {
    auto totalStart = chrono::high_resolution_clock::now();
    cout << "========================================\n";
    cout << "   JOB MATCHING SYSTEM (Linked List + Selection Sort + Linear Search)\n";
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

    Node* head = nullptr;
    int size = 0;

    auto loadStart = chrono::high_resolution_clock::now();
    ifstream file(filename);
    bool skipHeader = isJob;
    while (getline(file, token)) {
        if (token.empty()) continue;
        if (skipHeader) { skipHeader = false; continue; }
        
        Resume res;
        parseSkills(token, res, isJob);
        if (res.numSkills > 0) {
            insertAtEnd(head, res);
            ++size;
        }
    }
    file.close();
    auto loadEnd = chrono::high_resolution_clock::now();
    auto loadDur = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();

    cout << "File: " << filename << endl;
    cout << "Loaded Entries: " << size << endl;
    cout << "Load Time: " << loadDur << " ms\n";

    cout << "\n----------------------------------------\n";
    cout << "Sorting Data (Selection Sort)\n";
    cout << "----------------------------------------\n";

    auto sortStart = chrono::high_resolution_clock::now();
    selectionSortList(head);
    auto sortEnd = chrono::high_resolution_clock::now();
    auto sortDur = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();

    cout << "Sort Completed in " << sortDur << " ms\n";
    cout << "Selection Sort Stats:\n";
    cout << "   - Comparisons: " << g_selectionComparisons << endl;
    cout << "   - Swaps: " << g_selectionSwaps << endl;

    cout << "\n----------------------------------------\n";
    cout << "Matching Process\n";
    cout << "----------------------------------------\n";

    const int MAX_MATCHES = 10000;
    Match* matches = new Match[MAX_MATCHES];
    int mSize = 0;
    long long linTimeNs = 0, linCount = 0;
    auto matchStart = chrono::high_resolution_clock::now();

    matchLinkedList(head, userSkills, userNum, matches, mSize, jobTitle, isEmployer,
                    linTimeNs, linCount);

    auto matchEnd = chrono::high_resolution_clock::now();
    auto matchDur = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();

    cout << "Match Completed in " << matchDur << " ms\n";
    if (linCount > 0) {
        cout << "Linear Search Stats:\n";
        cout << "   - Total Searches: " << linCount << endl;
        cout << "   - Total Time: " << (linTimeNs / 1'000'000.0) << " ms\n";
        cout << "   - Avg Time/Search: " << (linTimeNs / (double)linCount) << " ns\n";
    }

    // Memory usage tracking
    cout << "\n----------------------------------------\n";
    cout << "Memory Usage\n";
    cout << "----------------------------------------\n";
    
    // macOS memory tracking (commented out)
    // #ifdef __APPLE__
    // struct mach_task_basic_info info;
    // mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    // if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) == KERN_SUCCESS) {
    //     cout << "Memory Used (Resident): " << (info.resident_size / (1024.0 * 1024.0)) << " MB\n";
    // }
    // #endif

    // Windows memory tracking
    #ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        cout << "Memory Used (Working Set): " << (pmc.WorkingSetSize / (1024.0 * 1024.0)) << " MB\n";
        cout << "Peak Memory Used: " << (pmc.PeakWorkingSetSize / (1024.0 * 1024.0)) << " MB\n";
    } else {
        cout << "Unable to retrieve memory usage information.\n";
    }
    #else
    cout << "Memory tracking not available on this platform (enable macOS code above if needed).\n";
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

    cout << "\n🏁 TOTAL EXECUTION TIME: " << totalDur << " ms\n";

    deleteList(head);
    delete[] matches;
    return 0;
}