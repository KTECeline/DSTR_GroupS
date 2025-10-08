#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>
using namespace std;

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
    regex pattern(R"((\w+\s+\w+)\s+needed\s+with\s+experience\s+in\s+([^\.]+))", regex_constants::icase);
    smatch match;

    while (getline(fin, line)) {
        if (line.front() == '"' && line.back() == '"')
            line = line.substr(1, line.size() - 2);

        if (regex_search(line, match, pattern)) {
            string job = toLower(match[1]);
            string skills = toLower(match[2]);
            fout << job << "," << skills << "\n";
        }
    }

    cout << "✅ Cleaned file saved as cleaned_job_description.csv" << endl;
    return 0;
}
