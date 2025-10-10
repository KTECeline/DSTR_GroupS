// list_merge.cpp
#include "common.hpp"

void runListMerge(const string userSkills[], int userNum, const string& filename, bool isEmployer) {
    bool isJob = !isEmployer;
    cout << "\n--- Linked List + Merge Sort + Binary Search ---" << endl;

    auto loadStart = chrono::high_resolution_clock::now();
    Node* head = nullptr;
    loadList(head, filename, isJob);
    auto loadEnd = chrono::high_resolution_clock::now();
    auto loadDur = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();
    int size = countList(head);
    cout << "Load time: " << loadDur << " ms (" << size << " entries)" << endl;

    auto sortStart = chrono::high_resolution_clock::now();
    if (head) head = mergeSortList(head);
    auto sortEnd = chrono::high_resolution_clock::now();
    auto sortDur = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();
    cout << "Sort time (Merge Sort): " << sortDur << " ms" << endl;

    auto matchStart = chrono::high_resolution_clock::now();
    // Use manual heap buffer for matches to avoid std::vector
    Match* matches = new Match[10000];
    int mSize = 0;
    matchList(head, userSkills, userNum, matches, mSize, true);
    auto matchEnd = chrono::high_resolution_clock::now();
    auto matchDur = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchDur << " ms" << endl;

    if (mSize > 0) printMatches(matches, mSize, isEmployer, size);
    else printMatches(nullptr, 0, isEmployer, size);
    delete[] matches;

    freeList(head);
}