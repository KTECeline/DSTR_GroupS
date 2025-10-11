// list_selection.cpp
#include "common.hpp"
#include "performance.hpp"

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
    PerformanceMeasurer measurer;
    loadTime = measurer.end("Load", countList(head));
    
    auto selectionSortWrapper = [](Node* head) -> Node* {
        selectionSortList(head);
        return head;
    };
    
    auto result = measureAndExecuteOperationsList(
        head,
        countList(head),
        userSkills,
        userNum,
        isEmployer,
        jobTitle,
        selectionSortWrapper,
        loadTime,
        sortTime,
        matchTime,
        false  // Use linear search for selection sort
    );
    
    int size = countList(head);
    printMatches(result.matches, result.mSize, isEmployer, size);
    writeMatchesToFile(result.matches, result.mSize, isEmployer, "matches_list_selection.txt");

    delete[] result.matches;
    freeList(head);
}