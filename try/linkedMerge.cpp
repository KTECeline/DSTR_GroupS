// 3_linkedMerge.cpp
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

struct Node {
    Entry data;
    Node* next;
};

static Node* createNode(const std::string& d) {
    Node* newNode = new Node();
    newNode->data.desc = d;
    newNode->next = nullptr;
    return newNode;
}

static void insertEnd(Node*& h, const std::string& d) {
    Node* newNode = createNode(d);
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
    if (left->data.desc <= right->data.desc) {
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

static int binarySearchLL(Node* h, const std::string& target) {
    int len = getLength(h);
    int low = 0, high = len - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        Node* curr = h;
        for (int i = 0; i < mid; ++i) {
            curr = curr->next;
        }
        if (curr->data.desc == target) return mid;
        if (curr->data.desc < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void linkedMergeMatch(const std::string& title, const std::string& required) {
    Node* head = nullptr;

    std::ifstream file("cleaned_resumes.txt");
    if (!file.is_open()) {
        std::cout << "Error opening cleaned_resumes.txt" << std::endl;
        return;
    }
    std::string line;
    auto start = std::chrono::high_resolution_clock::now();
    while (std::getline(file, line)) {
        if (!line.empty()) {
            insertEnd(head, line);
        }
    }
    file.close();
    auto end = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Load time: " << loadTime.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    head = mergeSort(head);
    end = std::chrono::high_resolution_clock::now();
    auto sortTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Sort time: " << sortTime.count() << " ms" << std::endl;

    // Demonstrate binary search
    if (head) {
        std::string firstDesc;
        Node* temp = head;
        firstDesc = temp->data.desc;
        int idx = binarySearchLL(head, firstDesc);
        std::cout << "Binary search demo: Found first resume at index " << idx << std::endl;
    }

    int totalReq = getNumSkills(required);
    if (totalReq == 0) {
        std::cout << "No skills entered." << std::endl;
        return;
    }

    std::cout << "For job: " << title << std::endl;
    std::cout << "Matched candidates:" << std::endl;
    int matchedCount = 0;
    Node* temp = head;
    while (temp) {
        std::string skillPart = getSkillPart(temp->data.desc);
        int matchC = getMatchCount(skillPart, required);
        double percent = static_cast<double>(matchC) / totalReq * 100.0;
        if (percent > 0.0) {
            std::cout << "Resume: " << temp->data.desc.substr(0, 100) << "... Match: " << percent << "%" << std::endl;
            ++matchedCount;
        }
        temp = temp->next;
    }
    if (matchedCount == 0) std::cout << "No matches found." << std::endl;
}