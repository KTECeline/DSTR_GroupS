#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>
using namespace std;

// Convert string to lowercase using STL algorithm transform
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

int main() {
    ifstream fin("job_description.csv");
    ofstream fout("cleaned_job_description.csv");
    fout << "job_title,skills\n";

    string line;
    getline(fin, line); // skip header
    // Complex regex pattern to extract job title and required skills
    // Format: captures two-word job title followed by skills after "needed with experience in"
    regex pattern(R"((\w+\s+\w+)\s+needed\s+with\s+experience\s+in\s+([^\.]+))", regex_constants::icase);
    smatch match;

    while (getline(fin, line)) {
        // Remove surrounding quotes
        // Uses basic string manipulation with boundary checking
        if (line.front() == '"' && line.back() == '"')
            line = line.substr(1, line.size() - 2);

        // Search for pattern matches using regex with capture groups
        // Extracts job title and skills from the text if pattern is found
        if (regex_search(line, match, pattern)) {
            string job = toLower(match[1]);
            string skills = toLower(match[2]);
            fout << job << "," << skills << "\n";
        }
    }

    cout << "✅ Cleaned file saved as cleaned_job_description.csv" << endl;
    return 0;
}
