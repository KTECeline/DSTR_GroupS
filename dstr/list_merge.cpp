// list_merge.cpp
#include "common.hpp"
#include "performance.hpp"

void runListMerge(const string userSkills[], int userNum, const string& filename, bool isEmployer, const string& jobTitle, double& loadTime, double& sortTime, double& matchTime) {
    bool isJob = !isEmployer;
    cout << "\n--- Linked List + Merge Sort + Binary Search ---" << endl;

    auto loadStart = chrono::high_resolution_clock::now();
    Node* head = nullptr;
    loadList(head, filename, isJob);
    PerformanceMeasurer measurer;
    loadTime = measurer.end("Load", countList(head));
    
    auto result = measureAndExecuteOperationsList(
        head,
        countList(head),
        userSkills,
        userNum,
        isEmployer,
        jobTitle,
        mergeSortList,
        loadTime,
        sortTime,
        matchTime,
        true  // Use binary search for merge sort
    );
    
    int size = countList(head);
    printMatches(result.matches, result.mSize, isEmployer, size);
    writeMatchesToFile(result.matches, result.mSize, isEmployer, "matches_list_merge.txt");
    
    delete[] result.matches;
    freeList(head);
}