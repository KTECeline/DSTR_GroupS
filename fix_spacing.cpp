#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

int main() {
    ifstream fin("dstr/cleaned_job_description.csv");
    ofstream fout("dstr/cleaned_job_description_temp.csv");
    string line;

    // Copy header as is
    getline(fin, line);
    fout << line << "\n";

    // Process remaining lines
    while (getline(fin, line)) {
        // Find position of first comma
        size_t pos = line.find(',');
        if (pos != string::npos) {
            // Split into job_title and skills
            string job_title = line.substr(0, pos);
            string skills = line.substr(pos + 1);
            
            // Remove spaces after commas in skills
            string cleaned_skills = regex_replace(skills, regex(",\\s+"), ",");
            
            // Write back to file
            fout << job_title << "," << cleaned_skills << "\n";
        }
    }

    fin.close();
    fout.close();

    // Replace original file with temp file
    remove("dstr/cleaned_job_description.csv");
    rename("dstr/cleaned_job_description_temp.csv", "dstr/cleaned_job_description.csv");

    cout << "Spaces after commas have been removed successfully" << endl;
    return 0;
}