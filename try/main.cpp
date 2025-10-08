#include <iostream>
#include <string>

// external helper + match functions
extern std::string normalize(std::string);
extern void arrayMergeMatch(const std::string& title, const std::string& required);
extern void arrayBinaryMatch(const std::string& title, const std::string& required);
extern void linkedMergeMatch(const std::string& title, const std::string& required);
extern void linkedBinaryMatch(const std::string& title, const std::string& required);

int main() {
    int choice;
    std::string title, required;

    std::cout << "=== Job Matching System ===\n";
    std::cout << "Select version:\n";
    std::cout << "1 - Array + Merge Sort\n";
    std::cout << "2 - Array + Binary Search\n";
    std::cout << "3 - Linked List + Merge Sort\n";
    std::cout << "4 - Linked List + Binary Search\n";
    std::cout << "Enter choice (1-4): ";
    std::cin >> choice;
    std::cin.ignore(); // clear newline

    std::cout << "\nEnter job title: ";
    std::getline(std::cin, title);

    std::cout << "Enter required skills (comma-separated): ";
    std::getline(std::cin, required);

    std::cout << "\n=== Matching Results ===\n";

    switch (choice) {
        case 1:
            arrayMergeMatch(title, required);
            break;
        case 2:
            arrayBinaryMatch(title, required);
            break;
        case 3:
            linkedMergeMatch(title, required);
            break;
        case 4:
            linkedBinaryMatch(title, required);
            break;
        default:
            std::cout << "Invalid choice.\n";
            break;
    }

    std::cout << "\n=========================\n";
    return 0;
}
