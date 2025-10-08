#include <iostream>
#include <fstream>
#include <string>
#include <cctype> // for tolower()

using namespace std;

// make lowercase
string toLower(string s) {
    for (char &c : s) c = tolower(c);
    return s;
}

// trim spaces and newline
string trim(string s) {
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\r' || s.front() == '\n' || s.front() == '"'))
        s.erase(s.begin());
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n' || s.back() == '"'))
        s.pop_back();
    return s;
}

int main() {
    ifstream fin("job_description.csv");
    if (!fin.is_open()) {
        cout << "Error: Cannot open job_description.csv\n";
        return 1;
    }

    ofstream fout("cleaned_job_description.csv");
    if (!fout.is_open()) {
        cout << "Error: Cannot create cleaned_job_description.csv\n";
        return 1;
    }

    fout << "job_title,skills\n"; // header line

    string line;
    int cleaned = 0;

    while (getline(fin, line)) {
        if (line.empty()) continue;

        // remove extra quotes or spaces
        line = trim(line);

        // find job title
        size_t posNeeded = line.find(" needed ");
        if (posNeeded == string::npos) continue;

        string job = trim(line.substr(0, posNeeded));
        job = toLower(job);

        // find start of skills
        size_t posExp = line.find("with experience in ", posNeeded);
        if (posExp == string::npos) continue;

        posExp += 19; // length of "with experience in "
        size_t posEnd = line.find('.', posExp);
        if (posEnd == string::npos) posEnd = line.size();

        string skills = trim(line.substr(posExp, posEnd - posExp));
        skills = toLower(skills);

        fout << job << "," << skills << "\n";
        cleaned++;
    }

    fin.close();
    fout.close();

    cout << "✅ Done! Cleaned " << cleaned << " job descriptions.\n";
    cout << "Output: cleaned_job_description.csv\n";

    return 0;
}
