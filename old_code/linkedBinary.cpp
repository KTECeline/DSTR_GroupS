// 4_linkedBinary.cpp
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
    double score;
};

struct Job {
    std::string title;
    std::string skills;
};

struct Node {
    Entry data;
    Node* next;
};

static Node* createNode(const std::string& d, double s = 0.0) {
    Node* newNode = new Node();
    newNode->data.desc = d;
    newNode->data.score = s;
    newNode->next = nullptr;
    return newNode;
}

static void insertEnd(Node*& h, const std::string& d, double s) {
    Node* newNode = createNode(d, s);
    if (!h) {
        h = newNode;
        return;
    }
    Node* temp = h;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
}

static Node* merge(Node* left, Node* right) {
    if (!left) return right;
    if (!right) return left;
    if (left->data.score >= right->data.score) {  // Descending
        left->next = merge(left->next, right);
        return left;
    } else {
        right->next = merge(right, left->next);
        return right;
    }
}

static Node* mergeSort(Node* h) {
    if (!h || !h->next) return h;
    Node* slow = h;
    Node* fast = h->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    Node* second = slow->next;
    slow->next = nullptr;
    Node* l = mergeSort(h);
    Node* r = mergeSort(second);
    return merge(l, r);
}

static int getLength(Node* h) {
    int len = 0;
    Node* temp = h;
    while (temp) {
        ++len;
        temp = temp->next;
    }
    return len;
}

static int findFirstLowScoreLL(Node* h) {
    int len = getLength(h);
    int low = 0, high = len - 1;
    int result = len;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        Node* curr = h;
        for (int i = 0; i < mid; ++i) {
            curr = curr->next;
        }
        if (curr->data.score <= 0.0) {
            result = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return result;
}

void linkedBinaryMatch() {
    const int MAXJ = 100;
    Job jobs[MAXJ];
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

    Node* head = nullptr;

    // Load resumes
    std::ifstream file("cleaned_resumes.txt");
    if (!file.is_open()) {
        std::cout << "Error opening cleaned_resumes.txt" << std::endl;
        return;
    }
    std::string line;
    auto start = std::chrono::high_resolution_clock::now();
    while (std::getline(file, line)) {
        if (!line.empty()) {
            insertEnd(head, line, 0.0);
        }
    }
    file.close();
    auto end = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Load time: " << loadTime.count() << " ms" << std::endl;

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

    std::cout << "For job: " << title << std::endl;

    int totalReq = getNumSkills(required);
    if (totalReq == 0) {
        std::cout << "No skills entered." << std::endl;
        return;
    }

    // Compute scores
    Node* temp = head;
    while (temp) {
        std::string skillPart = getSkillPart(temp->data.desc);
        int matchC = getMatchCount(skillPart, required);
        temp->data.score = static_cast<double>(matchC) / totalReq * 100.0;
        temp = temp->next;
    }

    start = std::chrono::high_resolution_clock::now();
    head = mergeSort(head);
    end = std::chrono::high_resolution_clock::now();
    auto sortTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Sort time (by score descending): " << sortTime.count() << " ms" << std::endl;

    // Use binary search to find cutoff
    int cutoff = findFirstLowScoreLL(head);
    std::cout << "Matched candidates (using binary search cutoff):" << std::endl;
    temp = head;
    for (int i = 0; i < cutoff; ++i) {
        std::cout << "Resume: " << temp->data.desc.substr(0, 100) << "... Match: " << temp->data.score << "%" << std::endl;
        temp = temp->next;
    }
    if (cutoff == 0) std::cout << "No matches found." << std::endl;
}