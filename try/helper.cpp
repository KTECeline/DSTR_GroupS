// helper.cpp
#include <string>
#include <algorithm>
#include <cctype>

std::string normalize(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c){ return std::tolower(c); });
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    return s;
}

std::string getSkillPart(const std::string& desc) {
    return desc;
}

int getNumSkills(const std::string& requiredStr) {
    std::string norm = normalize(requiredStr);
    int count = 0;
    size_t pos = 0;
    while (pos < norm.length()) {
        size_t next = norm.find(',', pos);
        if (next == std::string::npos) {
            if (!norm.substr(pos).empty()) ++count;
            break;
        } else {
            std::string token = norm.substr(pos, next - pos);
            if (!token.empty()) ++count;
            pos = next + 1;
        }
    }
    return count;
}

int getMatchCount(const std::string& skillPart, const std::string& requiredStr) {
    std::string norm_part = normalize(skillPart);
    std::string norm_req = normalize(requiredStr);
    int count = 0;
    size_t pos = 0;
    while (pos < norm_req.length()) {
        size_t next = norm_req.find(',', pos);
        std::string token;
        if (next == std::string::npos) {
            token = norm_req.substr(pos);
            pos = norm_req.length();
        } else {
            token = norm_req.substr(pos, next - pos);
            pos = next + 1;
        }
        if (!token.empty() && norm_part.find(token) != std::string::npos) ++count;
    }
    return count;
}