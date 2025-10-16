#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

// Data Structures
struct JobNode {
    std::string jobTitle;
    std::vector<std::string> skills;
    JobNode* next;
    
    JobNode(const std::string& title, const std::vector<std::string>& skillSet) 
        : jobTitle(title), skills(skillSet), next(nullptr) {}
};

struct ResumeNode {
    int resumeId;
    std::vector<std::string> skills;
    ResumeNode* next;
    
    ResumeNode(int id, const std::vector<std::string>& skillSet) 
        : resumeId(id), skills(skillSet), next(nullptr) {}
};

struct MatchResult {
    std::string jobTitle;
    int resumeId;
    int matchScore;
    double matchPercentage;
    
    MatchResult(const std::string& job, int resume, int score, double percentage)
        : jobTitle(job), resumeId(resume), matchScore(score), matchPercentage(percentage) {}
};

// Job Matcher Class
class LinkedListJobMatcher {
private:
    JobNode* jobHead;
    ResumeNode* resumeHead;
    int jobCount;
    int resumeCount;
    
    // Merge sort for jobs
    JobNode* getMiddle(JobNode* head) {
        if (!head) return head;
        JobNode* slow = head;
        JobNode* fast = head;
        JobNode* prev = nullptr;
        
        while (fast && fast->next) {
            prev = slow;
            slow = slow->next;
            fast = fast->next->next;
        }
        
        if (prev) prev->next = nullptr;
        return slow;
    }
    
    JobNode* mergeJobs(JobNode* left, JobNode* right) {
        if (!left) return right;
        if (!right) return left;
        
        JobNode* result = nullptr;
        
        if (left->jobTitle <= right->jobTitle) {
            result = left;
            result->next = mergeJobs(left->next, right);
        } else {
            result = right;
            result->next = mergeJobs(left, right->next);
        }
        
        return result;
    }
    
    JobNode* mergeSortJobs(JobNode* head) {
        if (!head || !head->next) return head;
        
        JobNode* middle = getMiddle(head);
        JobNode* left = head;
        JobNode* right = middle;
        
        left = mergeSortJobs(left);
        right = mergeSortJobs(right);
        
        return mergeJobs(left, right);
    }
    
    // Merge sort for resumes
    ResumeNode* getMiddleResume(ResumeNode* head) {
        if (!head) return head;
        ResumeNode* slow = head;
        ResumeNode* fast = head;
        ResumeNode* prev = nullptr;
        
        while (fast && fast->next) {
            prev = slow;
            slow = slow->next;
            fast = fast->next->next;
        }
        
        if (prev) prev->next = nullptr;
        return slow;
    }
    
    ResumeNode* mergeResumes(ResumeNode* left, ResumeNode* right) {
        if (!left) return right;
        if (!right) return left;
        
        ResumeNode* result = nullptr;
        
        if (left->skills.size() >= right->skills.size()) {
            result = left;
            result->next = mergeResumes(left->next, right);
        } else {
            result = right;
            result->next = mergeResumes(left, right->next);
        }
        
        return result;
    }
    
    ResumeNode* mergeSortResumes(ResumeNode* head) {
        if (!head || !head->next) return head;
        
        ResumeNode* middle = getMiddleResume(head);
        ResumeNode* left = head;
        ResumeNode* right = middle;
        
        left = mergeSortResumes(left);
        right = mergeSortResumes(right);
        
        return mergeResumes(left, right);
    }
    
    // Binary search
    JobNode* binarySearchJob(const std::string& jobTitle) {
        std::vector<JobNode*> jobArray;
        JobNode* current = jobHead;
        while (current) {
            jobArray.push_back(current);
            current = current->next;
        }
        
        int left = 0, right = jobArray.size() - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            
            if (jobArray[mid]->jobTitle == jobTitle) {
                return jobArray[mid];
            } else if (jobArray[mid]->jobTitle < jobTitle) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        
        return nullptr;
    }
    
    // Utility functions
    std::vector<std::string> parseSkills(const std::string& skillsStr) {
        std::vector<std::string> skills;
        std::stringstream ss(skillsStr);
        std::string skill;
        
        while (std::getline(ss, skill, ',')) {
            skill.erase(0, skill.find_first_not_of(" \t\r\n"));
            skill.erase(skill.find_last_not_of(" \t\r\n") + 1);
            if (!skill.empty()) {
                skills.push_back(skill);
            }
        }
        
        return skills;
    }
    
    void sortSkills(std::vector<std::string>& skills) {
        std::sort(skills.begin(), skills.end());
    }
    
    int calculateMatchScore(const std::vector<std::string>& jobSkills, 
                           const std::vector<std::string>& resumeSkills) {
        int matchCount = 0;
        size_t i = 0, j = 0;
        
        while (i < jobSkills.size() && j < resumeSkills.size()) {
            if (jobSkills[i] == resumeSkills[j]) {
                matchCount++;
                i++;
                j++;
            } else if (jobSkills[i] < resumeSkills[j]) {
                i++;
            } else {
                j++;
            }
        }
        
        return matchCount;
    }
    
public:
    LinkedListJobMatcher() : jobHead(nullptr), resumeHead(nullptr), jobCount(0), resumeCount(0) {}
    
    ~LinkedListJobMatcher() {
        while (jobHead) {
            JobNode* temp = jobHead;
            jobHead = jobHead->next;
            delete temp;
        }
        
        while (resumeHead) {
            ResumeNode* temp = resumeHead;
            resumeHead = resumeHead->next;
            delete temp;
        }
    }
    
    void loadJobsFromCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        std::string line;
        std::getline(file, line); // Skip header
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string jobTitle, skillsStr;
            
            if (std::getline(ss, jobTitle, ',') && std::getline(ss, skillsStr)) {
                std::vector<std::string> skills = parseSkills(skillsStr);
                sortSkills(skills);
                
                JobNode* newJob = new JobNode(jobTitle, skills);
                newJob->next = jobHead;
                jobHead = newJob;
                jobCount++;
            }
        }
        
        file.close();
        std::cout << "Loaded " << jobCount << " jobs from " << filename << std::endl;
    }
    
    void loadResumesFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        std::string line;
        int resumeId = 1;
        
        while (std::getline(file, line)) {
            if (!line.empty()) {
                std::vector<std::string> skills = parseSkills(line);
                sortSkills(skills);
                
                ResumeNode* newResume = new ResumeNode(resumeId++, skills);
                newResume->next = resumeHead;
                resumeHead = newResume;
                resumeCount++;
            }
        }
        
        file.close();
        std::cout << "Loaded " << resumeCount << " resumes from " << filename << std::endl;
    }
    
    void sortJobsByTitle() {
        jobHead = mergeSortJobs(jobHead);
    }
    
    void sortResumesBySkillCount() {
        resumeHead = mergeSortResumes(resumeHead);
    }
    
    std::vector<MatchResult> findMatches() {
        std::vector<MatchResult> matches;
        
        JobNode* jobCurrent = jobHead;
        while (jobCurrent) {
            ResumeNode* resumeCurrent = resumeHead;
            while (resumeCurrent) {
                int matchScore = calculateMatchScore(jobCurrent->skills, resumeCurrent->skills);
                
                if (matchScore > 0) {
                    double matchPercentage = (double)matchScore / jobCurrent->skills.size() * 100.0;
                    matches.emplace_back(jobCurrent->jobTitle, resumeCurrent->resumeId, 
                                       matchScore, matchPercentage);
                }
                
                resumeCurrent = resumeCurrent->next;
            }
            jobCurrent = jobCurrent->next;
        }
        
        std::sort(matches.begin(), matches.end(), 
                  [](const MatchResult& a, const MatchResult& b) {
                      return a.matchScore > b.matchScore;
                  });
        
        return matches;
    }
    
    std::vector<MatchResult> findMatchesForJob(const std::string& jobTitle) {
        std::vector<MatchResult> matches;
        
        JobNode* job = binarySearchJob(jobTitle);
        if (!job) {
            std::cout << "Job '" << jobTitle << "' not found!" << std::endl;
            return matches;
        }
        
        ResumeNode* resumeCurrent = resumeHead;
        while (resumeCurrent) {
            int matchScore = calculateMatchScore(job->skills, resumeCurrent->skills);
            
            if (matchScore > 0) {
                double matchPercentage = (double)matchScore / job->skills.size() * 100.0;
                matches.emplace_back(job->jobTitle, resumeCurrent->resumeId, 
                                   matchScore, matchPercentage);
            }
            
            resumeCurrent = resumeCurrent->next;
        }
        
        std::sort(matches.begin(), matches.end(), 
                  [](const MatchResult& a, const MatchResult& b) {
                      return a.matchScore > b.matchScore;
                  });
        
        return matches;
    }
    
    void displayJobs() {
        std::cout << "\n=== JOBS ===" << std::endl;
        JobNode* current = jobHead;
        int count = 0;
        
        while (current && count < 10) {
            std::cout << "Job: " << current->jobTitle << std::endl;
            std::cout << "Skills: ";
            for (size_t i = 0; i < current->skills.size(); ++i) {
                std::cout << current->skills[i];
                if (i < current->skills.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl << std::endl;
            
            current = current->next;
            count++;
        }
        
        if (jobCount > 10) {
            std::cout << "... and " << (jobCount - 10) << " more jobs" << std::endl;
        }
    }
    
    void displayResumes() {
        std::cout << "\n=== RESUMES ===" << std::endl;
        ResumeNode* current = resumeHead;
        int count = 0;
        
        while (current && count < 10) {
            std::cout << "Resume ID: " << current->resumeId << std::endl;
            std::cout << "Skills: ";
            for (size_t i = 0; i < current->skills.size(); ++i) {
                std::cout << current->skills[i];
                if (i < current->skills.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl << std::endl;
            
            current = current->next;
            count++;
        }
        
        if (resumeCount > 10) {
            std::cout << "... and " << (resumeCount - 10) << " more resumes" << std::endl;
        }
    }
    
    void displayMatches(const std::vector<MatchResult>& matches) {
        std::cout << "\n=== TOP MATCHES ===" << std::endl;
        
        for (size_t i = 0; i < std::min(matches.size(), size_t(20)); ++i) {
            std::cout << "Job: " << matches[i].jobTitle 
                      << " | Resume ID: " << matches[i].resumeId
                      << " | Score: " << matches[i].matchScore
                      << " | Match: " << std::fixed << std::setprecision(1) 
                      << matches[i].matchPercentage << "%" << std::endl;
        }
        
        if (matches.size() > 20) {
            std::cout << "... and " << (matches.size() - 20) << " more matches" << std::endl;
        }
    }
    
    int getJobCount() const { return jobCount; }
    int getResumeCount() const { return resumeCount; }
};

// Main Program
void displayMenu() {
    std::cout << "\n=== JOB MATCHING SYSTEM (LINKED LIST) ===" << std::endl;
    std::cout << "1. Load Data" << std::endl;
    std::cout << "2. Display Jobs" << std::endl;
    std::cout << "3. Display Resumes" << std::endl;
    std::cout << "4. Sort Data" << std::endl;
    std::cout << "5. Find All Matches" << std::endl;
    std::cout << "6. Find Matches for Specific Job" << std::endl;
    std::cout << "7. Performance Analysis" << std::endl;
    std::cout << "8. Exit" << std::endl;
    std::cout << "Choose an option: ";
}

void performanceComparison() {
    std::cout << "\n=== PERFORMANCE COMPARISON ===" << std::endl;
    
    LinkedListJobMatcher matcher;
    
    std::cout << "Loading data..." << std::endl;
    auto loadStart = std::chrono::high_resolution_clock::now();
    matcher.loadJobsFromCSV("cleaned_job_description.csv");
    matcher.loadResumesFromFile("cleaned_resumes.txt");
    auto loadEnd = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(loadEnd - loadStart);
    
    std::cout << "\n--- LINKED LIST PERFORMANCE ---" << std::endl;
    std::cout << "Data Loading Time: " << loadTime.count() << " ms" << std::endl;
    
    auto sortStart = std::chrono::high_resolution_clock::now();
    matcher.sortJobsByTitle();
    matcher.sortResumesBySkillCount();
    auto sortEnd = std::chrono::high_resolution_clock::now();
    auto sortTime = std::chrono::duration_cast<std::chrono::milliseconds>(sortEnd - sortStart);
    
    std::cout << "Merge Sort Time: " << sortTime.count() << " ms" << std::endl;
    
    auto matchStart = std::chrono::high_resolution_clock::now();
    auto matches = matcher.findMatches();
    auto matchEnd = std::chrono::high_resolution_clock::now();
    auto matchTime = std::chrono::duration_cast<std::chrono::milliseconds>(matchEnd - matchStart);
    
    std::cout << "Job Matching Time: " << matchTime.count() << " ms" << std::endl;
    std::cout << "Total Matches Found: " << matches.size() << std::endl;
    
    std::cout << "\n--- ALGORITHM ---" << std::endl;
    std::cout << "Merge Sort: O(n log n)" << std::endl;
    std::cout << "Binary Search: O(log n)" << std::endl;
    std::cout << "Job Matching: O(n * m * k)" << std::endl;
}

int main() {
    LinkedListJobMatcher matcher;
    int choice;
    bool dataLoaded = false;
    
    std::cout << "=== LINKED LIST JOB MATCHING COMPONENT ===" << std::endl;
    std::cout << "Optimized with Merge Sort and Binary Search" << std::endl;
    
    while (true) {
        displayMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                std::cout << "\nLoading data..." << std::endl;
                auto start = std::chrono::high_resolution_clock::now();
                
                matcher.loadJobsFromCSV("cleaned_job_description.csv");
                matcher.loadResumesFromFile("cleaned_resumes.txt");
                
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                std::cout << "Data loaded successfully in " << duration.count() << " ms" << std::endl;
                dataLoaded = true;
                break;
            }
            
            case 2: {
                if (!dataLoaded) {
                    std::cout << "Please load data first!" << std::endl;
                    break;
                }
                matcher.displayJobs();
                break;
            }
            
            case 3: {
                if (!dataLoaded) {
                    std::cout << "Please load data first!" << std::endl;
                    break;
                }
                matcher.displayResumes();
                break;
            }
            
            case 4: {
                if (!dataLoaded) {
                    std::cout << "Please load data first!" << std::endl;
                    break;
                }
                
                std::cout << "\nSorting data using Merge Sort..." << std::endl;
                auto start = std::chrono::high_resolution_clock::now();
                
                matcher.sortJobsByTitle();
                matcher.sortResumesBySkillCount();
                
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                std::cout << "Data sorted successfully in " << duration.count() << " ms" << std::endl;
                break;
            }
            
            case 5: {
                if (!dataLoaded) {
                    std::cout << "Please load data first!" << std::endl;
                    break;
                }
                
                std::cout << "\nFinding all matches..." << std::endl;
                auto start = std::chrono::high_resolution_clock::now();
                
                auto matches = matcher.findMatches();
                
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                std::cout << "Found " << matches.size() << " matches in " 
                          << duration.count() << " ms" << std::endl;
                matcher.displayMatches(matches);
                break;
            }
            
            case 6: {
                if (!dataLoaded) {
                    std::cout << "Please load data first!" << std::endl;
                    break;
                }
                
                std::cout << "Enter job title to search: ";
                std::cin.ignore();
                std::string jobTitle;
                std::getline(std::cin, jobTitle);
                
                std::cout << "\nSearching for matches using Binary Search..." << std::endl;
                auto start = std::chrono::high_resolution_clock::now();
                
                auto matches = matcher.findMatchesForJob(jobTitle);
                
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                std::cout << "Found " << matches.size() << " matches in " 
                          << duration.count() << " ms" << std::endl;
                matcher.displayMatches(matches);
                break;
            }
            
            case 7: {
                performanceComparison();
                break;
            }
            
            case 8: {
                std::cout << "Exiting..." << std::endl;
                return 0;
            }
            
            default: {
                std::cout << "Invalid choice! Please try again." << std::endl;
                break;
            }
        }
    }
    
    return 0;
}