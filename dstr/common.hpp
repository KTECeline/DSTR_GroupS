// common.hpp
#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Resume {
    string fullDesc;
    string title;
    string skills[20];
    int numSkills;
};

struct Node {
    Resume data;
    Node* next;
};

struct Match {
    int id;
    double perc;
    string fullDesc;
};

// Function declarations
void parseSkills(string line, Resume& res, bool isJob);
bool binarySearchSkill(const string sk[20], int n, const string& target);
bool linearSearchSkill(const string sk[20], int n, const string& target);
void matchArray(const Resume arr[], int size, const string userSk[], int userN, Match m[], int& mSize, bool useBin);
void matchList(Node* head, const string userSk[], int userN, Match m[], int& mSize, bool useBin);
void printMatches(const Match m[], int mSize, bool isEmployer);

Node* mergeList(Node* a, Node* b);
Node* mergeSortList(Node* head);
void loadList(Node*& head, const string filename, bool isJob);
void freeList(Node* head);
int countList(Node* head);

void runArrayMerge(const string userSkills[], int userNum, const string& filename, bool isEmployer);
void runArraySelection(const string userSkills[], int userNum, const string& filename, bool isEmployer);
void runListMerge(const string userSkills[], int userNum, const string& filename, bool isEmployer);
void runListSelection(const string userSkills[], int userNum, const string& filename, bool isEmployer);
void runAll(const string userSkills[], int userNum, const string& filename, bool isEmployer);

#endif