// helper.cpp
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;

// Normalize: lowercase + remove all spaces
string normalize(string s) {
    // Convert to lowercase safely
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return tolower(c); });

    // Remove spaces and tabs
    s.erase(remove_if(s.begin(), s.end(),
        [](unsigned char c) { return isspace(c); }),
        s.end());

    return s;
}

// Return the skills part directly (CSV string)
string getSkillPart(const string& desc) {
    return desc;
}

// Count number of comma-separated skills
int getNumSkills(const string& requiredStr) {
    string norm = normalize(requiredStr);
    if (norm.empty()) return 0;

    int count = 0;
    size_t pos = 0;

    while (pos < norm.length()) {
        size_t next = norm.find(',', pos);
        string token = (next == string::npos)
            ? norm.substr(pos)
            : norm.substr(pos, next - pos);

        if (!token.empty()) ++count;
        if (next == string::npos) break;
        pos = next + 1;
    }
    return count;
}

// Count how many required skills appear in candidate's skills
int getMatchCount(const string& skillPart, const string& requiredStr) {
    string norm_part = normalize(skillPart);
    string norm_req = normalize(requiredStr);
    if (norm_part.empty() || norm_req.empty()) return 0;

    int count = 0;
    size_t pos = 0;

    while (pos < norm_req.length()) {
        size_t next = norm_req.find(',', pos);
        string token = (next == string::npos)
            ? norm_req.substr(pos)
            : norm_req.substr(pos, next - pos);

        if (!token.empty() && norm_part.find(token) != string::npos)
            ++count;

        if (next == string::npos) break;
        pos = next + 1;
    }
    return count;
}
