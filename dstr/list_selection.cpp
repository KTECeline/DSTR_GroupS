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

void runListSelection(const string userSkills[], int userNum, const string& filename, bool isEmployer) {
    bool isJob = !isEmployer;
    cout << "\n--- Linked List + Selection Sort + Linear Search ---" << endl;

    auto loadStart = chrono::high_resolution_clock::now();
    Node* head = nullptr;
    loadList(head, filename, isJob);
    auto loadEnd = chrono::high_resolution_clock::now();
    auto loadDur = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();
    int size = countList(head);
    cout << "Load time: " << loadDur << " ms (" << size << " entries)" << endl;

    auto sortStart = chrono::high_resolution_clock::now();
    selectionSortList(head);
    auto sortEnd = chrono::high_resolution_clock::now();
    auto sortDur = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();
    cout << "Sort time (Selection Sort): " << sortDur << " ms" << endl;

    auto matchStart = chrono::high_resolution_clock::now();
    Match matches[10000];
    int mSize = 0;
    matchList(head, userSkills, userNum, matches, mSize, false);
    auto matchEnd = chrono::high_resolution_clock::now();
    auto matchDur = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchDur << " ms" << endl;

    printMatches(matches, mSize, isEmployer, size);

    freeList(head);
}