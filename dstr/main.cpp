// main.cpp
#include "common.hpp"

void runAll(const string userSkills[], int userNum, const string& filename, bool isEmployer) {
    runArrayMerge(userSkills, userNum, filename, isEmployer);
    runArraySelection(userSkills, userNum, filename, isEmployer);
    runListMerge(userSkills, userNum, filename, isEmployer);
    runListSelection(userSkills, userNum, filename, isEmployer);
}

int main() {
    cout << "Job Matching System" << endl;
    
    char role;
    cout << "Are you an Employer (e) looking for candidates or Employee (m) looking for jobs? ";
    cin >> role;
    cin.ignore();
    bool isEmployer = (role == 'e' || role == 'E');
    bool isJob = !isEmployer;
    string filename = isEmployer ? "cleaned_resumes.txt" : "cleaned_job_description.csv";
    string skillsPrompt = isEmployer ? "Enter Required Skills for the job (comma-separated): " : "Enter Your Skills (comma-separated): ";
    cout << skillsPrompt;
    string skillsStr;
    getline(cin, skillsStr);

    stringstream ss(skillsStr);
    string token;
    string userSkills[20];
    int userNum = 0;
    while (getline(ss, token, ',')) {
        if (!token.empty()) {
            size_t start = token.find_first_not_of(" \t");
            if (start != string::npos) {
                size_t end = token.find_last_not_of(" \t");
                userSkills[userNum++] = token.substr(start, end - start + 1);
            }
        }
    }

    int choice;
    cout << "Choose version:\n1. Array + Merge Sort + Binary Search\n2. Array + Selection Sort + Linear Search\n3. Linked List + Merge Sort + Binary Search\n4. Linked List + Selection Sort + Linear Search\n5. Run All & Compare" << endl;
    cin >> choice;
    cin.ignore();

    switch (choice) {
        case 1: runArrayMerge(userSkills, userNum, filename, isEmployer); break;
        case 2: runArraySelection(userSkills, userNum, filename, isEmployer); break;
        case 3: runListMerge(userSkills, userNum, filename, isEmployer); break;
        case 4: runListSelection(userSkills, userNum, filename, isEmployer); break;
        case 5: runAll(userSkills, userNum, filename, isEmployer); break;
        default: cout << "Invalid choice." << endl;
    }
    return 0;
}