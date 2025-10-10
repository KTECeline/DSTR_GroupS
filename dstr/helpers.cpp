// helpers.cpp
#include "common.hpp"

void parseSkills(string line, Resume& res, bool isJob) {
    res.numSkills = 0;
    res.fullDesc = line;
    stringstream ss(line);
    string token;
    vector<string> tokens;
    while (getline(ss, token, ',')) {
        if (!token.empty()) {
            token.erase(0, token.find_first_not_of(" \t"));
            size_t last = token.find_last_not_of(" \t");
            if (last != string::npos) token.erase(last + 1);
            tokens.push_back(token);
        }
    }
    if (isJob) {
        if (tokens.empty()) return;
        res.title = tokens[0];
        for (size_t i = 1; i < tokens.size() && res.numSkills < 20; ++i) {
            res.skills[res.numSkills++] = tokens[i];
        }
    } else {
        res.title = "Candidate";
        for (size_t i = 0; i < tokens.size() && res.numSkills < 20; ++i) {
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

void matchArray(const Resume arr[], int size, const string userSk[], int userN, Match m[], int& mSize, bool useBin) {
    mSize = 0;
    for (int i = 0; i < size; ++i) {
        int matched = 0;
        for (int j = 0; j < userN; ++j) {
            bool found = useBin ? binarySearchSkill(arr[i].skills, arr[i].numSkills, userSk[j])
                                : linearSearchSkill(arr[i].skills, arr[i].numSkills, userSk[j]);
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

void matchList(Node* head, const string userSk[], int userN, Match m[], int& mSize, bool useBin) {
    mSize = 0;
    int id = 0;
    Node* curr = head;
    while (curr) {
        int matched = 0;
        for (int j = 0; j < userN; ++j) {
            bool found = useBin ? binarySearchSkill(curr->data.skills, curr->data.numSkills, userSk[j])
                                : linearSearchSkill(curr->data.skills, curr->data.numSkills, userSk[j]);
            if (found) ++matched;
        }
        double perc = userN > 0 ? (static_cast<double>(matched) / userN) * 100.0 : 0.0;
        if (perc > 0) {
            m[mSize].id = id;
            m[mSize].perc = perc;
            m[mSize].fullDesc = curr->data.fullDesc;
            ++mSize;
        }
        ++id;
        curr = curr->next;
    }
    // Bubble sort matches by perc descending
    for (int i = 0; i < mSize - 1; ++i) {
        for (int j = 0; j < mSize - i - 1; ++j) {
            if (m[j].perc < m[j + 1].perc) swap(m[j], m[j + 1]);
        }
    }
}

void printMatches(const Match m[], int mSize, bool isEmployer) {
    string header = isEmployer ? "Matching Candidates:" : "Matching Jobs:";
    string label = isEmployer ? "Candidate " : "Job ";
    cout << header << endl;
    for (int i = 0; i < mSize; ++i) {
        cout << label << (i + 1) << ": " << m[i].fullDesc << " - " 
             << fixed << setprecision(2) << m[i].perc << "% match" << endl;
    }
    if (mSize == 0) cout << "No matches found." << endl;
}

Node* mergeList(Node* a, Node* b) {
    Node* dummy = new Node{Resume(), nullptr};
    Node* tail = dummy;
    while (a && b) {
        if (a->data.numSkills >= b->data.numSkills) {
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = (a ? a : b);
    Node* res = dummy->next;
    delete dummy;
    return res;
}

Node* mergeSortList(Node* head) {
    if (!head || !head->next) return head;
    Node* slow = head;
    Node* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    Node* midNext = slow->next;
    slow->next = nullptr;
    Node* left = mergeSortList(head);
    Node* right = mergeSortList(midNext);
    return mergeList(left, right);
}

void loadList(Node*& head, const string filename, bool isJob) {
    head = nullptr;
    ifstream file(filename);
    string line;
    Node* tail = nullptr;
    bool skipHeader = isJob;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (skipHeader) {
            skipHeader = false;
            continue;
        }
        Node* newNode = new Node();
        parseSkills(line, newNode->data, isJob);
        if (newNode->data.numSkills == 0) {
            delete newNode;
            continue;
        }
        newNode->next = nullptr;
        if (!head) head = newNode;
        else tail->next = newNode;
        tail = newNode;
    }
    file.close();
}

void freeList(Node* head) {
    while (head) {
        Node* next = head->next;
        delete head;
        head = next;
    }
}

int countList(Node* head) {
    int cnt = 0;
    while (head) {
        ++cnt;
        head = head->next;
    }
    return cnt;
}