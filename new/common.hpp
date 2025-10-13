// common.hpp
#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <cctype>

using namespace std;

struct Resume {
    string fullDesc;
    string title;
    string skills[20];
    int numSkills;
};

struct Match {
    int id;
    double perc;
    string fullDesc;
};

// Function declarations (supporting only)
void parseSkills(string line, Resume& res, bool isJob);
// Search helpers
bool binarySearchSkill(const string sk[20], int n, const string& target);
bool linearSearchSkill(const string sk[20], int n, const string& target);
void printMatches(const Match m[], int mSize, bool isEmployer, int totalSize);

#endif