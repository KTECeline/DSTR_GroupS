// main.cpp
#include <iostream>
#include <string>

extern void arrayMergeMatch();
extern void arrayBinaryMatch();
extern void linkedMergeMatch();
extern void linkedBinaryMatch();

int main() {
    int choice;
    std::cout << "Choose version:\n1 - Array + Merge Sort\n2 - Array + Binary Search\n3 - Linked List + Merge Sort\n4 - Linked List + Binary Search\n";
    std::cin >> choice;
    std::cin.ignore();  // Clear newline for getline in functions

    switch (choice) {
        case 1:
            arrayMergeMatch();
            break;
        case 2:
            arrayBinaryMatch();
            break;
        case 3:
            linkedMergeMatch();
            break;
        case 4:
            linkedBinaryMatch();
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
    }
    return 0;
}