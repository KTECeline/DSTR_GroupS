#include <iostream>
#include <fstream>
#include <string>
#include <cctype> // for tolower()

using namespace std;

// Convert string to lowercase for consistent matching
// Example: "Power BI" becomes "power bi"
string toLower(string s) {
    for (char &c : s) c = tolower(c);
    return s;
}

// Remove whitespace and quotes from both ends of string
// Handles: spaces, tabs, line breaks, and quote marks
// Example: '  "sql, power bi"  \n' becomes "sql, power bi"
string trim(string s) {
    // Remove junk from the start
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\r' || s.front() == '\n' || s.front() == '"'))
        s.erase(s.begin());
    
    // Remove junk from the end
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\r' || s.back() == '\n' || s.back() == '"'))
        s.pop_back();
    
    return s;
}

int main() {
    // Try to open the input file
    ifstream fin("job_description.csv");
    if (!fin.is_open()) {
        cout << "Error: Cannot open job_description.csv\n";
        return 1;
    }

    // Create the output file for cleaned data
    ofstream fout("cleaned_job_description.csv");
    if (!fout.is_open()) {
        cout << "Error: Cannot create cleaned_job_description.csv\n";
        return 1;
    }

    // Write CSV header
    fout << "job_title,skills\n";

    string line;
    int cleaned = 0; // Counter for successfully processed entries

    // Process each line from the input file
    while (getline(fin, line)) {
        if (line.empty()) continue; // Skip blank lines

        line = trim(line); // Remove extra whitespace/quotes

        size_t posNeeded = line.find(" needed ");
        if (posNeeded == string::npos) continue; // Skip if pattern not found

        string job = trim(line.substr(0, posNeeded));
        job = toLower(job);

        // Extract skills section
        // Find where "with experience in " starts
        size_t posExp = line.find("with experience in ", posNeeded);
        if (posExp == string::npos) continue;

        // Skip past "with experience in " (19 characters)
        posExp += 19;
        
        // Find where the sentence ends (period character)
        size_t posEnd = line.find('.', posExp);
        if (posEnd == string::npos) posEnd = line.size();

        // Clean the skills string
        // Extract: "SQL, Power BI, well, try, Excel, Tableau"
        // This gets ALL text between "with experience in" and the period
        // Note: Garbage words like "well, try" are kept at this stage
        // Further filtering happens in later processing steps
        string skills = trim(line.substr(posExp, posEnd - posExp));
        skills = toLower(skills);

        // Write cleaned output
        // Format: job_title,skills
        // Example output: "data analyst,sql,power bi,excel,tableau"
        fout << job << "," << skills << "\n";
        cleaned++;
    }

    fin.close();
    fout.close();

    // Display results
    cout << "✅ Done! Cleaned " << cleaned << " job descriptions.\n";
    cout << "Output: cleaned_job_description.csv\n";

    return 0;
}