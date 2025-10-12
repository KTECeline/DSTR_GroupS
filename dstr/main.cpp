// main.cpp
#include "common.hpp"

void runAll(const string userSkills[], int userNum, const string& filename, bool isEmployer, const string& jobTitle) {
    double am_load, am_sort, am_match;
    double as_load, as_sort, as_match;
    double lm_load, lm_sort, lm_match;
    double ls_load, ls_sort, ls_match;

    cout << "\n=== Running Array Merge Sort ===\n";
    runArrayMerge(userSkills, userNum, filename, isEmployer, jobTitle, am_load, am_sort, am_match);
    
    cout << "\n=== Running Array Selection Sort ===\n";
    runArraySelection(userSkills, userNum, filename, isEmployer, jobTitle, as_load, as_sort, as_match);
    
    cout << "\n=== Running List Merge Sort ===\n";
    runListMerge(userSkills, userNum, filename, isEmployer, jobTitle, lm_load, lm_sort, lm_match);
    
    cout << "\n=== Running List Selection Sort ===\n";
    runListSelection(userSkills, userNum, filename, isEmployer, jobTitle, ls_load, ls_sort, ls_match);

    // Simple ASCII table for comparison
    cout << "\nPerformance Comparison Table (ms):" << endl;
    cout << "Method                  | Load  | Sort  | Match" << endl;
    cout << "------------------------|-------|-------|------" << endl;
    cout << "Array Merge + Bin Search| " << fixed << setprecision(0) << am_load << "   | " << am_sort << "   | " << am_match << endl;
    cout << "Array Selection + Lin Sr| " << as_load << "   | " << as_sort << "   | " << as_match << endl;
    cout << "List Merge + Bin Search | " << lm_load << "   | " << lm_sort << "   | " << lm_match << endl;
    cout << "List Selection + Lin Sr | " << ls_load << "   | " << ls_sort << "   | " << ls_match << endl;
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
    string jobTitle = "";
    if (!isEmployer) {
        cout << "Enter Job Title to match: ";
        getline(cin, jobTitle);
    }
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
        case 1: {
            double dummy;
            runArrayMerge(userSkills, userNum, filename, isEmployer, jobTitle, dummy, dummy, dummy);
            break;
        }
        case 2: {
            double dummy;
            runArraySelection(userSkills, userNum, filename, isEmployer, jobTitle, dummy, dummy, dummy);
            break;
        }
        case 3: {
            double dummy;
            runListMerge(userSkills, userNum, filename, isEmployer, jobTitle, dummy, dummy, dummy);
            break;
        }
        case 4: {
            double dummy;
            runListSelection(userSkills, userNum, filename, isEmployer, jobTitle, dummy, dummy, dummy);
            break;
        }
        case 5: runAll(userSkills, userNum, filename, isEmployer, jobTitle); break;
        default: cout << "Invalid choice." << endl;
    }
    return 0;
}