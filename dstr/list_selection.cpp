// list_selection.cpp
#include "common.hpp"

void selectionSortList(Node*& head) {
    if (!head) return;
    int count = countList(head);
    Node* start = head;
    for (int step = 0; step < count - 1; ++step) {
        Node* maxN = start;
        Node* curr = start->next;
        while (curr) {
            if (curr->data.numSkills > maxN->data.numSkills) {
                maxN = curr;
            }
            curr = curr->next;
        }
        swap(start->data, maxN->data);
        start = start->next;
    }
}

void runListSelection(const string userSkills[], int userNum, const string& filename, bool isEmployer, const string& jobTitle, double& loadTime, double& sortTime, double& matchTime) {
    bool isJob = !isEmployer;
    cout << "\n--- Linked List + Selection Sort + Linear Search ---" << endl;

    auto loadStart = chrono::high_resolution_clock::now();
    Node* head = nullptr;
    loadList(head, filename, isJob);
    auto loadEnd = chrono::high_resolution_clock::now();
    loadTime = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();
    int size = countList(head);
    cout << "Load time: " << loadTime << " ms (" << size << " entries)" << endl;

    auto sortStart = chrono::high_resolution_clock::now();
    selectionSortList(head);
    auto sortEnd = chrono::high_resolution_clock::now();
    sortTime = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();
    cout << "Sort time: " << sortTime << " ms" << endl;

    auto matchStart = chrono::high_resolution_clock::now();
    const int MAX_SIZE = 10000;
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    matchList(head, userSkills, userNum, matches, mSize, false, jobTitle, isEmployer);
    auto matchEnd = chrono::high_resolution_clock::now();
    matchTime = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchTime << " ms" << endl;

    printMatches(matches, mSize, isEmployer, size);

    delete[] matches;
    freeList(head);
}