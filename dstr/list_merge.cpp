// list_merge.cpp
#include "common.hpp"

void runListMerge(const string userSkills[], int userNum, const string& filename, bool isEmployer, const string& jobTitle, double& loadTime, double& sortTime, double& matchTime) {
    bool isJob = !isEmployer;
    cout << "\n--- Linked List + Merge Sort + Binary Search ---" << endl;

    auto loadStart = chrono::high_resolution_clock::now();
    Node* head = nullptr;
    loadList(head, filename, isJob);
    auto loadEnd = chrono::high_resolution_clock::now();
    loadTime = chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count();
    int size = countList(head);
    cout << "Load time: " << loadTime << " ms (" << size << " entries)" << endl;

    auto sortStart = chrono::high_resolution_clock::now();
    if (head) head = mergeSortList(head);
    auto sortEnd = chrono::high_resolution_clock::now();
    sortTime = chrono::duration_cast<chrono::milliseconds>(sortEnd - sortStart).count();
    cout << "Sort time: " << sortTime << " ms" << endl;

    auto matchStart = chrono::high_resolution_clock::now();
    const int MAX_SIZE = 10000;
    Match* matches = new Match[MAX_SIZE];
    int mSize = 0;
    matchList(head, userSkills, userNum, matches, mSize, true, jobTitle, isEmployer);
    auto matchEnd = chrono::high_resolution_clock::now();
    matchTime = chrono::duration_cast<chrono::milliseconds>(matchEnd - matchStart).count();
    cout << "Match time: " << matchTime << " ms" << endl;

    printMatches(matches, mSize, isEmployer, size);

    delete[] matches;
    freeList(head);
}