#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

// QUESTION MANAGEMENT
void addQuestion(int id, const string& text, int evaluationId, const string& clo, const string& filename)
{
    ofstream outFile(filename, ios::app);
    if (outFile.is_open())
    {
        outFile << id << "," << text << "," << evaluationId << "," << clo << endl;
        outFile.close();
        cout << "Question added successfully." << endl;
    }
    else
    {
        cerr << "Error: Unable to open file " << filename << " for writing." << endl;
    }
}
void updateQuestion(int id, const string& newText, const string& newCLO, const string& filename)
{
    ifstream inFile(filename);
    ofstream tempFile("temp_" + filename);
    if (inFile.is_open() && tempFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string token;
            getline(ss, token, ',');
            int readID = stoi(token);
            if (readID == id)
            {
                tempFile << id << "," << newText << "," << newCLO << endl;
            }
            else
            {
                tempFile << line << endl;
            }
        }
        inFile.close();
        tempFile.close();
        remove(filename.c_str());
        rename(("temp_" + filename).c_str(), filename.c_str());
        cout << "Question updated successfully." << endl;
    }
    else
    {
        cerr << "Error: Unable to open file " << filename << " for reading or writing." << endl;
    }
}
void deleteQuestion(int id, const string& filename)
{
    ifstream inFile(filename);
    ofstream tempFile("temp_" + filename);
    if (inFile.is_open() && tempFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string token;
            getline(ss, token, ',');
            int readID = stoi(token);
            if (readID != id)
            {
                tempFile << line << endl;
            }
        }
        inFile.close();
        tempFile.close();
        remove(filename.c_str());
        rename(("temp_" + filename).c_str(), filename.c_str());
        cout << "Question deleted successfully." << endl;
    }
    else
    {
        cerr << "Error: Unable to open file " << filename << " for reading or writing." << endl;
    }
}
bool verifyQuestions(int evaluationId, const string& filename)
{
    ifstream inFile(filename);
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string token;
            getline(ss, token, ',');
            int readID = stoi(token);
            if (readID != evaluationId)
            {
                continue;
            }
            // Move to the CLO part
            for (int i = 0; i < 2; ++i)
            {
                getline(ss, token, ',');
            }
            if (token.empty())
            {
                cerr << "Error: Question with ID " << readID << " does not test any CLO." << endl;
                return false;
            }
        }
        inFile.close();
    }
    else
    {
        cerr << "Error: Unable to open file " << filename << " for reading." << endl;
    }
    return true;
}
void viewAllQuestions(const string& filename)
{
    ifstream inFile(filename);
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            cout << line << endl;
        }
        inFile.close();
    }
    else
    {
        cerr << "Error: Unable to open file " << filename << " for reading." << endl;
    }
}
class Evaluation
{
private:
    int id;
    string type;
    int courseId;
public:
    Evaluation(int id, const string& type, int courseId)
        : id(id), type(type), courseId(courseId)
    {

    }
    // Getter methods
    int getID() const
    {
        return id;
    }
    string getType() const
    {
        return type;
    }
    int getCourseID() const
    {
        return courseId;
    }
    // Methods to update evaluation details
    void updateEvaluation(const string& newType, int newCourseId)
    {
        type = newType;
        courseId = newCourseId;
    }
    // Serialization method for file I/O
    string serialize() const
    {
        stringstream ss;
        ss << id << "," << type << "," << courseId;
        return ss.str();
    }
    // Deserialization method for file I/O
    static Evaluation deserialize(const string& data)
    {
        stringstream ss(data);
        string token;
        getline(ss, token, ',');
        int id = stoi(token);
        getline(ss, token, ',');
        string type = token;
        getline(ss, token, ',');
        int courseId = stoi(token);
        return Evaluation(id, type, courseId);
    }
};
class EvaluationManager
{
private:
    vector<Evaluation> evaluations;
public:
    // Method to add a new evaluation
    void addEvaluation(int id, const string& type, int courseId)
    {
        evaluations.push_back(Evaluation(id, type, courseId));
    }
    // Method to update an existing evaluation
    void updateEvaluation(int id, const string& newType, int newCourseId)
    {
        auto it = find_if(evaluations.begin(), evaluations.end(), [&](const Evaluation& eval)
            {
                return eval.getID() == id;
            });
        if (it != evaluations.end())
        {
            it->updateEvaluation(newType, newCourseId);
        }
    }
    // Method to delete an evaluation
    void deleteEvaluation(int id)
    {
        evaluations.erase(
            remove_if(evaluations.begin(), evaluations.end(), [&](const Evaluation& eval) {
                return eval.getID() == id;
                }),
            evaluations.end());
    }
    // Method to view all evaluations
    void viewAllEvaluations() const
    {
        for (const auto& eval : evaluations)
        {
            cout << "Evaluation ID: " << eval.getID() << endl;
            cout << "Type: " << eval.getType() << endl;
            cout << "Course ID: " << eval.getCourseID() << endl << endl;
        }
    }
    // Method to read evaluations from a file
    void readFromFile(const string& filename)
    {
        ifstream inFile(filename);
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                evaluations.push_back(Evaluation::deserialize(line));
            }
            inFile.close();
        }
        else
        {
            cerr << "Error: Unable to open file " << filename << " for reading." << endl;
        }
    }

    // Method to write evaluations to a file
    void writeToFile(const string& filename) const
    {
        ofstream outFile(filename);
        if (outFile.is_open())
        {
            for (const auto& eval : evaluations)
            {
                outFile << eval.serialize() << endl;
            }
            outFile.close();
        }
        else
        {
            cerr << "Error: Unable to open file " << filename << " for writing." << endl;
        }
    }
};
//course management
void deleteEvaluationsByCourseId(int courseId)
{
    ifstream inFile("evaluations.txt");
    ofstream tempFile("temp_evaluations.txt");
    if (!inFile.is_open() || !tempFile.is_open())
    {
        cerr << "Error: Unable to open evaluations.txt or temp_evaluations.txt for reading or writing." << endl;
        return;
    }
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read evaluation ID
        int evaluationId = stoi(token);
        getline(ss, token, ','); // Read evaluation type
        getline(ss, token, ','); // Read course ID
        int readCourseId = stoi(token);
        if (readCourseId != courseId)
        {
            tempFile << line << endl; // Copy line to temp file if course ID doesn't match
        }
    }
    inFile.close();
    tempFile.close();
    // Remove original file
    remove("evaluations.txt");
    // Rename temp file to original file
    rename("temp_evaluations.txt", "evaluations.txt");
    cout << "Evaluations for course with ID " << courseId << " deleted successfully." << endl;
}
bool isProgramExists(int programID)
{
    ifstream inFile("programs.txt");
    if (!inFile.is_open()) {
        cerr << "Error opening programs.txt" << endl;
        return false;
    }
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string programIDStr;
        getline(ss, programIDStr, ',');
        int id = stoi(programIDStr);
        if (id == programID)
        {
            inFile.close();
            return true;
        }
    }
    inFile.close();
    return false;
}
bool courseExists(int programID, int courseID)
{
    ifstream inFile("courses.txt");
    if (!inFile.is_open())
    {
        cerr << "Error: Unable to open courses.txt for reading." << endl;
        return false;
    }
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read program ID
        int readProgramID = stoi(token);
        getline(ss, token, ','); // Read course ID
        int readCourseID = stoi(token);
        if (readProgramID == programID && readCourseID == courseID)
        {
            inFile.close();
            return true; // Course found
        }
    }
    inFile.close();
    return false; // Course not found
}
void addCourseToProgram(int programID)
{
    // Check if the program exists
    if (!isProgramExists(programID))
    {
        cerr << "Error: Program with ID " << programID << " does not exist." << endl;
        return;
    }
    int courseID;
    string courseName;
    string courseDescription;
    // Get course ID from user input
    cout << "Enter course ID to add to the program: ";
    cin >> courseID;
    // Check if the course already exists
    if (courseExists(programID, courseID))
    {
        cerr << "Error: Course with ID " << courseID << " already exists for program ID " << programID << "." << endl;
        return;
    }
    // Get course name from user input
    cout << "Enter course name: ";
    cin.ignore(); // Ignore the newline character left in the buffer
    getline(cin, courseName);
    cout << "Enter course Description: ";
    cin.ignore(); // Ignore the newline character left in the buffer
    getline(cin, courseDescription);
    // Open "courses.txt" file in append mode
    ofstream outFile("courses.txt", ios::app);
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open courses.txt for writing." << endl;
        return;
    }
    // Write program ID, course ID, and course name to file
    outFile << programID << "," << courseID << "," << courseName << "," << courseDescription << endl;
    outFile.close();
    cout << "Course added to program successfully." << endl;
}
void viewAllCourses(int programID)
{
    ifstream inFile("courses.txt");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open courses.txt for reading." << endl;
        return;
    }
    bool found = false;
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read program ID
        int readProgramID = stoi(token);
        if (readProgramID == programID)
        {
            found = true;
            getline(ss, token, ','); // Read course ID
            int readCourseID = stoi(token);
            cout << "Program ID: " << readProgramID << endl;
            cout << "Course ID: " << readCourseID << endl;
            getline(ss, token, ','); // Read course name
            cout << "Course Name: " << token << endl;
            getline(ss, token, ','); // Read course description
            cout << "Course Description: " << token << endl << endl;
        }
    }
    inFile.close();
    if (!found)
    {
        cerr << "No courses found for program with ID " << programID << "." << endl;
    }
}
void updateCourse()
{
    int programID, courseID;
    string newName, desc;
    cout << "Enter Program ID: ";
    cin >> programID;
    cout << "Enter Course ID: ";
    cin >> courseID;
    cin.ignore();  // Clear the newline character from the input buffer
    if (!courseExists(programID, courseID))
    {
        cerr << "Error: Course with ID " << courseID << " does not exist for program with ID " << programID << "." << endl;
        return;
    }
    cout << "Enter new course name: ";
    getline(cin, newName);
    cout << "Enter new course description: ";
    getline(cin, desc);
    ifstream inFile("courses.txt");
    ofstream tempFile("temp_courses.txt");
    if (!inFile.is_open() || !tempFile.is_open())
    {
        cerr << "Error: Unable to open courses.txt or temp_courses.txt for reading or writing." << endl;
        return;
    }
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read program ID
        int readProgramID = stoi(token);
        getline(ss, token, ','); // Read course ID
        int readCourseID = stoi(token);
        if (readProgramID == programID && readCourseID == courseID)
        {
            tempFile << programID << "," << courseID << "," << newName << "," << desc << endl; // Update course details
            cout << "Course updated successfully." << endl;
        }
        else {
            tempFile << line << endl; // Copy unchanged line to temp file
        }
    }

    inFile.close();
    tempFile.close();

    // Remove original file
    remove("courses.txt");

    // Rename temp file to original file
    rename("temp_courses.txt", "courses.txt");
}
void removeCourse(int programID, int courseID)
{
    ifstream inFile("courses.txt");
    ofstream tempFile("temp_courses.txt");

    if (!inFile.is_open() || !tempFile.is_open())
    {
        cerr << "Error: Unable to open courses.txt or temp_courses.txt for reading or writing." << endl;
        return;
    }

    bool found = false;
    bool programExists = false;
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read program ID
        int readProgramID = stoi(token);
        getline(ss, token, ','); // Read course ID
        int readCourseID = stoi(token);
        if (readProgramID == programID)
        {
            programExists = true;
            if (readCourseID == courseID)
            {
                deleteEvaluationsByCourseId(courseID);
                found = true;
                cout << "Course with ID " << courseID << " removed successfully." << endl;
                continue; // Skip writing this line to temp file
            }
        }
        tempFile << line << endl; // Copy unchanged line to temp file
    }

    inFile.close();
    tempFile.close();

    // Remove original file
    remove("courses.txt");

    // Rename temp file to original file
    rename("temp_courses.txt", "courses.txt");

    if (!isProgramExists(programID))
    {
        cerr << "Error: Program with ID " << programID << " does not exist." << endl;
    }
    else if (!found) {
        cerr << "Error: Course with ID " << courseID << " does not exist for program with ID " << programID << "." << endl;
    }
}

//programs managemet
bool isPLOExists(int ploID);
bool isProgramIDDuplicated(int programID) {
    ifstream inFile("programs.txt");
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string token;
            getline(ss, token, ','); // Read program ID
            int readProgramID = stoi(token);
            if (readProgramID == programID) {
                inFile.close();
                return true; // Found a duplicate
            }
        }
        inFile.close();
    }
    else {
        cerr << "Error: Unable to open programs.txt for reading." << endl;
    }
    return false; // No duplicate found
}
void removePLO(int programID)
{
    ifstream inFile("plos.txt");
    ofstream tempFile("temp_plos.txt");

    if (!inFile.is_open() || !tempFile.is_open())
    {
        cerr << "Error: Unable to open plos.txt or temp_plos.txt for reading or writing." << endl;
        return;
    }

    bool found = false;
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read program ID
        int readProgramID = stoi(token);
        if (readProgramID == programID)
        {
            // Skip writing this line to the temporary file
            found = true;
            continue;
        }
        // Write unchanged line to the temporary file
        tempFile << line << endl;
    }

    // Close the files
    inFile.close();
    tempFile.close();

    // Replace the original file with the temporary file
    remove("plos.txt");
    rename("temp_plos.txt", "plos.txt");


}
void removeCoursesForProgram(int programID)
{
    // Open the courses.txt file for reading
    ifstream inFile("courses.txt");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open courses.txt for reading." << endl;
        return;
    }

    // Open a temporary file for writing
    ofstream tempFile("temp_courses.txt");
    if (!tempFile.is_open()) {
        cerr << "Error: Unable to open temp_courses.txt for writing." << endl;
        inFile.close();
        return;
    }

    string line;
    bool removed = false;
    // Read each line from the courses.txt file
    while (getline(inFile, line))
    {
        // Parse the line to extract programID
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read programID
        int readProgramID = stoi(token);

        // Check if the current line matches the specified programID
        if (readProgramID == programID)
        {
            removed = true;
            // Skip writing this line to the temporary file
            continue;
        }

        // Write the current line to the temporary file
        tempFile << line << endl;
    }

    // Close the files
    inFile.close();
    tempFile.close();

    // Remove the original courses.txt file
    remove("courses.txt");

    // Rename the temporary file to courses.txt
    rename("temp_courses.txt", "courses.txt");

    // If no courses were found for the specified program, display an error message
    if (!removed)
    {
        cerr << "Error: No courses found for program with ID " << programID << "." << endl;
    }
}
void updateProgramInFile(int id, const string& newName, const string& newDescription)
{
    ifstream inFile("programs.txt");
    ofstream outFile("programs_temp.txt");

    if (inFile.is_open() && outFile.is_open())
    {
        string line;
        bool updated = false;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string programIDStr;
            getline(ss, programIDStr, ',');
            int programID = stoi(programIDStr);
            if (programID == id)
            {
                // Update program information
                outFile << id << "," << newName << "," << newDescription << endl;
                cout << "Program updated successfully." << endl;
                updated = true;
            }
            else
            {
                // Copy unchanged line to temp file
                outFile << line << endl;
            }
        }
        inFile.close();
        outFile.close();

        if (!updated)
        {
            cout << "Program with ID " << id << " not found." << endl;
            // Remove temp file since no update occurred
            remove("programs_temp.txt");
        }
        else {
            // Remove original file
            remove("programs.txt");
            // Rename temp file to original file
            rename("programs_temp.txt", "programs.txt");
        }
    }
    else
    {
        cerr << "Error: Unable to open file for reading or writing." << endl;
    }
}
void removeProgramFromFile(int id)
{
    ifstream inFile("programs.txt");
    ofstream outFile("programs_temp.txt");
    if (inFile.is_open() && outFile.is_open())
    {
        string line;
        bool removed = false;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string programIDStr;
            getline(ss, programIDStr, ',');
            int programID = stoi(programIDStr);
            if (programID == id)
            {
                removeCoursesForProgram(programID);
                removePLO(programID);
                removed = true;
                cout << "Program removed successfully." << endl;
            }
            else
            {
                // Copy unchanged line to temp file
                outFile << line << endl;
            }
        }
        inFile.close();
        outFile.close();

        // Remove original file
        remove("programs.txt");

        // Rename temp file to original file
        rename("programs_temp.txt", "programs.txt");
    }
    else
    {
        cerr << "Error: Unable to open file for reading or writing." << endl;
    }




}
void addProgramToFile()
{
    ofstream cFile("courses.txt", ios::app);
    ofstream outFile("programs.txt", ios::app);
    if (cFile.is_open() && outFile.is_open())
    {
        int id;
        string name, description;
        cout << "Enter program ID: ";
        cin >> id;
        if (isProgramIDDuplicated(id))
        {
            cout << "Duplicate ID Occurs Try with Different ID.\n";
            return;
        }
        cout << "Enter program name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter program description: ";
        getline(cin, description);
        //cFile << id << ",\n";
        outFile << id << "," << name << "," << description << endl;
        cFile.close();
        outFile.close();
        cout << "Program added successfully to file." << endl;
    }
    else
    {
        cerr << "Unable to open file for writing: programs.txt" << endl;
    }
}
void viewAllProgramsFromFile()
{
    ifstream inFile("programs.txt");
    if (inFile.is_open()) {
        cout << "All Programs:" << endl;
        string line;
        while (getline(inFile, line))
        {
            cout << line << endl;
        }
        inFile.close();
    }
    else
    {
        cerr << "Unable to open file for reading: programs.txt" << endl;
    }
}
//plo management
void createPLO(int programID)
{
    if (isProgramExists(programID))
    {
        int ploID;
        string ploDescription;

        cout << "Enter PLO ID: ";
        cin >> ploID;
        if (isPLOExists(ploID))
        {
            cout << "PLO ID Already Exist in the File.\n";
            return;
        }
        cout << "Enter PLO description: ";
        cin.ignore();
        getline(cin, ploDescription);
        // Open the file in append mode
        ofstream outFile("plos.txt", ios::app);
        if (!outFile.is_open())
        {
            cerr << "Error: Unable to open plos.txt for writing." << endl;
            return;
        }
        // Write the PLO details to the file
        outFile << programID << "," << ploID << "," << ploDescription << endl;
        outFile.close();
        cout << "New PLO added successfully." << endl;
    }
    else
    {
        cout << "The Program ID is  Invalid.\n";
    }
}
void updatePLODescription(int programID, int ploID)
{
    // Prompt the user to enter the new description
    string newDescription;
    cout << "Enter new PLO description: ";
    cin.ignore();
    getline(cin, newDescription);
    // Open the input and temporary files
    ifstream inFile("plos.txt");
    ofstream tempFile("temp_plos.txt");
    if (!inFile.is_open() || !tempFile.is_open()) {
        cerr << "Error: Unable to open plos.txt or temp_plos.txt for reading or writing." << endl;
        return;
    }
    // Read each line from the input file
    string line;
    bool found = false;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read program ID
        int readProgramID = stoi(token);
        getline(ss, token, ','); // Read PLO ID
        int readPLOID = stoi(token);
        string description;
        getline(ss, description); // Read PLO description

        // Check if the current line matches the provided program and PLO IDs
        if (readProgramID == programID && readPLOID == ploID)
        {
            // Update the description
            tempFile << programID << "," << ploID << "," << newDescription << endl;
            cout << "PLO description updated successfully." << endl;
            found = true;
        }
        else
        {
            // Write unchanged line to the temporary file
            tempFile << line << endl;
        }
    }
    // Close the files
    inFile.close();
    tempFile.close();

    // Replace the original file with the temporary file
    remove("plos.txt");
    rename("temp_plos.txt", "plos.txt");
    if (!found)
    {
        cerr << "Error: PLO with ID " << ploID << " not found for program with ID " << programID << "." << endl;
    }
}
void deletePLO(int programID, int ploID)
{
    // Open the input and temporary files
    ifstream inFile("plos.txt");
    ofstream tempFile("temp_plos.txt");

    if (!inFile.is_open() || !tempFile.is_open())
    {
        cerr << "Error: Unable to open plos.txt or temp_plos.txt for reading or writing." << endl;
        return;
    }
    // Read each line from the input file
    string line;
    bool found = false;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read program ID
        int readProgramID = stoi(token);
        getline(ss, token, ','); // Read PLO ID
        int readPLOID = stoi(token);

        // Check if the current line matches the provided program and PLO IDs
        if (readProgramID == programID && readPLOID == ploID)
        {
            // Skip writing this line to the temporary file
            found = true;
            continue;
        }
        // Write unchanged line to the temporary file
        tempFile << line << endl;
    }
    // Close the files
    inFile.close();
    tempFile.close();
    // Replace the original file with the temporary file
    remove("plos.txt");
    rename("temp_plos.txt", "plos.txt");
    if (!found)
    {
        cerr << "Error: PLO with ID " << ploID << " not found for program with ID " << programID << "." << endl;
    }
    else
    {
        cout << "PLO deleted successfully." << endl;
    }
}
void viewPLOs(int programID)
{
    ifstream inFile("plos.txt");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open plos.txt for reading." << endl;
        return;
    }

    bool found = false;
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read program ID
        int readProgramID = stoi(token);
        if (readProgramID == programID) {
            // Print PLO details
            cout << "Program ID: " << readProgramID << endl;
            getline(ss, token, ','); // Read PLO ID
            int readPLOID = stoi(token);
            cout << "PLO ID: " << readPLOID << endl;
            string description;
            getline(ss, description); // Read PLO description
            cout << "Description: " << description << endl << endl;;
            cout << endl;
            found = true;
        }
    }

    if (!found) {
        cerr << "Error: No PLOs found for program with ID " << programID << "." << endl;
    }

    inFile.close();
}

// clos management
void viewAllCLOForPLOAndCourseID(int ploID, int courseID) {
    // Check if PLO ID and Course ID are valid (greater than 0)
    if (ploID <= 0 || courseID <= 0) {
        cerr << "Error: Invalid PLO ID or Course ID." << endl;
        return;
    }

    ifstream inFile("clos.txt");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open clos.txt for reading." << endl;
        return;
    }

    cout << "Course Learning Outcomes (CLO) for PLO " << ploID << " and Course ID " << courseID << ":" << endl << endl;

    string line;
    bool found = false;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string token;

        getline(ss, token, ','); // Read CLO ID
        int cloID = stoi(token);

        getline(ss, token, ','); // Read CLO description
        string cloDescription = token;

        getline(ss, token, ','); // Read Course ID
        int readCourseID = stoi(token);

        getline(ss, token, ','); // Read PLO ID
        int readPLOID = stoi(token);

        if (readPLOID == ploID && readCourseID == courseID) {
            // Print the CLO ID and description
            cout << "CLO ID: " << cloID << endl;
            cout << "Description: " << cloDescription << endl;
            cout << endl; // Add a new line for readability
            found = true;
        }
    }

    inFile.close();

    if (!found) {
        cout << "No Course Learning Outcomes found for PLO " << ploID << " and Course ID " << courseID << endl;
    }
}
bool isCLOIDExists(int cloID) {
    ifstream inFile("clos.txt");
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string token;
            getline(ss, token, ','); // Read CLO ID
            int readCloID = stoi(token);
            if (readCloID == cloID) {
                inFile.close();
                return true;
            }
        }
        inFile.close();
    }
    return false;
}
bool isPLOExists(int ploID)
{
    ifstream inFile("plos.txt");
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string token;
            getline(ss, token, ','); // Read Program ID
            int readProgramID = stoi(token);
            getline(ss, token, ','); // Read PLO ID
            int readPLOID = stoi(token);
            if (readPLOID == ploID)
            {
                inFile.close();
                return true;
            }
        }
        inFile.close();
    }
    return false;
}
bool isCourseExists(int courseID)
{
    ifstream inFile("courses.txt");
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string token;
            getline(ss, token, ','); // Read Program ID
            int readProgramID = stoi(token);
            getline(ss, token, ','); // Read Course ID
            int readCourseID = stoi(token);
            if (readCourseID == courseID)
            {
                inFile.close();
                return true;
            }
        }
        inFile.close();
    }
    return false;
}
void createCLO(int cloID, const string& description, int courseID, int ploID) {
    if (!isCourseExists(courseID))
    {
        cerr << "Error: Course with ID " << courseID << " does not exist." << endl;
        return;
    }

    if (!isPLOExists(ploID))
    {
        cerr << "Error: PLO with ID " << ploID << " does not exist." << endl;
        return;
    }

    ofstream outFile("clos.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << cloID << "," << description << "," << courseID << "," << ploID << endl;
        outFile.close();
        cout << "CLO created successfully." << endl;
    }
    else
    {
        cerr << "Error: Unable to open clos.txt for writing." << endl;
    }
}
void updateCLODescription(int cloID, const string& newDescription)
{
    if (!isCLOIDExists(cloID)) {
        cout << "INVALID CLO ID and ClO ID not present in the File.\n";
        return;
    }

    ifstream inFile("clos.txt");
    ofstream tempFile("temp_clos.txt");

    if (!inFile.is_open() || !tempFile.is_open())
    {
        cerr << "Error: Unable to open clos.txt or temp_clos.txt for reading or writing." << endl;
        return;
    }

    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); // Read CLO ID
        int readCLOID = stoi(token);
        if (readCLOID == cloID)
        {
            // Write the updated line with the new description
            tempFile << cloID << "," << newDescription << ",";
            // Skip the old description
            getline(ss, token, ',');
            // Write the rest of the line (course ID, PLO ID, and add a comma at the end)
            while (getline(ss, token, ','))
            {
                tempFile << token << ",";
            }
            tempFile << endl; // Add a comma at the end of the line
            cout << "CLO description updated successfully." << endl;
        }
        else
        {
            tempFile << line << endl; // Copy unchanged line to temp file
        }
    }

    inFile.close();
    tempFile.close();

    // Remove original file
    remove("clos.txt");

    // Rename temp file to original file
    rename("temp_clos.txt", "clos.txt");
}
void removeCLO(int cloID)
{
    if (!isCLOIDExists(cloID))
    {
        cout << "The CLO is invalid Write Valid CLO .\n";
        return;
    }

    ifstream inFile("clos.txt");
    ofstream tempFile("temp_clos.txt");

    if (inFile.is_open() && tempFile.is_open())
    {
        string line;
        bool cloRemoved = false;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string token;
            getline(ss, token, ','); // Read CLO ID
            int readCloID = stoi(token);
            if (readCloID == cloID)
            {
                cloRemoved = true;
                cout << "CLO with ID " << cloID << " removed successfully." << endl;
            }
            else
            {
                tempFile << line << endl; // Copy unchanged line to temp file
            }
        }
        inFile.close();
        tempFile.close();

        // Remove original file
        remove("clos.txt");

        // Rename temp file to original file
        rename("temp_clos.txt", "clos.txt");

        if (!cloRemoved)
        {
            cerr << "Error: CLO with ID " << cloID << " not found." << endl;
        }
    }
    else
    {
        cerr << "Error: Unable to open clos.txt or temp_clos.txt for reading or writing." << endl;
    }
}// Function to remove an obsolete CLO
void ensureCLOAlignment()
{
    ifstream inFile("clos.txt");
    if (!inFile) {
        cerr << "Error: Unable to open clos.txt for reading." << endl;
        return;
    }

    string line;
    while (getline(inFile, line))
    {
        try
        {
            stringstream ss(line);
            string token;
            getline(ss, token, ','); // Read CLO ID
            int cloID = 0;
            if (!token.empty())
            {
                cloID = stoi(token);
            }
            else
            {
                cerr << "Error: CLO ID is empty." << endl;
                continue; // Move to the next line
            }
            getline(ss, token, ','); // Read CLO description
            string description = token;
            getline(ss, token, ','); // Read course ID
            int courseID = 0;
            if (!token.empty())
            {
                courseID = stoi(token);
            }
            else
            {
                cerr << "Error: Course ID is empty for CLO " << cloID << "." << endl;
                continue; // Move to the next line
            }
            getline(ss, token, ','); // Read PLO ID
            int ploID = 0;
            if (!token.empty())
            {
                ploID = stoi(token);
            }
            else
            {
                cerr << "Error: PLO ID is empty for CLO " << cloID << "." << endl;
                continue; // Move to the next line
            }

            // Check if course and PLO exist
            if (!isCourseExists(courseID))
            {
                cerr << "Error: Course with ID " << courseID << " for CLO " << cloID << " does not exist." << endl;
            }
            if (!isPLOExists(ploID)) {
                cerr << "Error: PLO with ID " << ploID << " for CLO " << cloID << " does not exist." << endl;
            }
        }
        catch (const exception& e) {
            cerr << "Error processing line from clos.txt: " << e.what() << endl;
        }
    }
    cout << "All CLOs are aligned with their corresponding courses and PLOs.\n";
    inFile.close();
}
//classes  
class PLO {
private:
    int id;
    vector<int> cloIDs; // IDs of the CLOs associated with this PLO
public:
    PLO(int id) : id(id) {}

    void addCLO(int cloID) {
        cloIDs.push_back(cloID);
    }

    const vector<int>& getCLOs() const {
        return cloIDs;
    }

    int getID() const {
        return id;
    }
};
class CLO {
private:
    int id;
    vector<int> ploIDs; // IDs of the PLOs associated with this CLO
public:
    CLO(int id) : id(id) {}

    int getCLOID() const {
        return id;
    }

    void addPLO(int ploID) {
        ploIDs.push_back(ploID);
    }

    const vector<int>& getPLOs() const {
        return ploIDs;
    }
};
class Course {
private:
    int id;
    vector<CLO> clos;
public:
    Course(int id) : id(id) {}

    int getCourseID() const {
        return id;
    }

    vector<CLO>& getCLOs() {
        return clos;
    }

    const vector<CLO>& getCLOs() const {
        return clos;
    }
};
class Program {
private:
    int id;
    string name;
    string description;
    vector<PLO> plos;
    vector<Course> courses;
    vector<Evaluation> evaluations;
public:
    Program(int id, const string& name, const string& description) : id(id), name(name), description(description) {}
    void serializeToFile(ofstream& outFile) const
    {
        outFile << id << "," << name << "," << description << endl;
    }

    void print() const {
        cout << "Program ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Description: " << description << endl;
    }

    int getProgramID() const {
        return id;
    }

    const string& getName() const {
        return name;
    }

    const string& getDescription() const {
        return description;
    }

    void setName(const string& newName) {
        name = newName;
    }

    void setDescription(const string& newDescription) {
        description = newDescription;
    }

    void addPLO(const PLO& plo) {
        plos.push_back(plo);
    }

    const vector<PLO>& getPLOs() const {
        return plos;
    }

    void addCourse(const Course& course) {
        courses.push_back(course);
    }

    const vector<Course>& getCourses() const {
        return courses;
    }

    void addEvaluation(const Evaluation& evaluation) {
        evaluations.push_back(evaluation);
    }

    const vector<Evaluation>& getEvaluations() const {
        return evaluations;
    }

    // Additional methods for program management
    void updateProgramDetails(const string& newName, const string& newDescription) {
        name = newName;
        description = newDescription;
    }

    void deleteProgram() {
        // Perform actions to delete the program, such as clearing vectors
        plos.clear();
        courses.clear();
        evaluations.clear();
    }
};
//interfaces
void CLOManagement()
{
    do
    {

        int opt;
        cout << "Choose Option: " << endl;
        cout << "1. View ALL Course Learning Outcome  (CLO) for Specific PLO and CourseID.\n";
        cout << "2. creates a new Course Learning Outcome (CLO).\n";
        cout << "3. updates existing CLO descriptions.\n";
        cout << "4.  removes CLOs if they're obsolete.\n";
        cout << "5. each CLO is aligned with a PLO and a course.\n";
        cin >> opt;
        if (opt == 1) {
            system("cls");
            int ploid, courseid;
            string description;
            cout << "Enter PLO ID.\n";
            cin >> ploid;
            cout << "Enter Course ID.\n";
            cin >> courseid;
            viewAllCLOForPLOAndCourseID(ploid, courseid);
            cout << endl;
        }
        if (opt == 2) {
            system("cls");
            int cloid, ploid, courseid;
            string description;
            cout << "Enter PLO ID.\n";
            cin >> ploid;
            cout << "Enter Course ID.\n";
            cin >> courseid;
            cout << "Enter CLO ID.\n";
            cin >> cloid;
            cout << "Enter CLOS Description.\n";
            cin.ignore();
            getline(cin, description);
            createCLO(cloid, description, courseid, ploid);
        }
        if (opt == 3) {
            system("cls");
            int cloid;
            string description;
            cout << "Enter CLO ID .\n";
            cin >> cloid;
            cout << "Enter CLO Description .\n";
            cin.ignore();
            getline(cin, description);
            updateCLODescription(cloid, description);
        }
        if (opt == 4)
        {
            system("cls");
            int cloid;
            cout << "Enter CLO ID .\n";
            cin >> cloid;
            removeCLO(cloid);
        }
        if (opt == 5)
        {
            system("cls");
            ensureCLOAlignment();
        }
        if (opt == 0) {
            system("cls");
            break;
        }

    } while (true);

}
void PLOManagement()
{
    do
    {

        int opt;
        cout << "Choose Option: " << endl;
        cout << "1. View PLO to specific Program.\n";
        cout << "2. creates a new Program Learning Outcome(PLO).\n";
        cout << "3. updates PLO descriptions.\n";
        cout << "4. deletes PLOs .\n";

        cin >> opt;
        if (opt == 2) {
            system("cls");
            int programid;
            cout << "Enter Program ID: ";
            cin >> programid;
            createPLO(programid);


        }
        if (opt == 3) {
            system("cls");
            int programid, ploid;
            cout << "Enter Program ID: ";
            cin >> programid;
            cout << "Enter PLO ID: ";
            cin >> ploid;
            updatePLODescription(programid, ploid);

        }
        if (opt == 4) {
            system("cls");
            int programid, ploid;
            cout << "Enter Program ID: ";
            cin >> programid;
            cout << "Enter PLO ID: ";
            cin >> ploid;
            deletePLO(programid, ploid);
        }
        if (opt == 1)
        {
            system("cls");
            int programid;
            cout << "Enter Program ID: ";
            cin >> programid;
            viewPLOs(programid);
        }
        if (opt == 0) {

            break;
        }

    } while (true);




}
void CourseManagement()
{
    do
    {

        int opt;
        cout << "Choose Option: " << endl;
        cout << "1. views a list of courses for a specific program.\n";
        cout << "2. adds new courses to the program.\n";
        cout << "3. updates existing course details.\n";
        cout << "4. removes a course from the program.\n";
        cout << "0. EXIT.\n";
        cin >> opt;
        if (opt == 1)
        {
            system("cls");
            int programid;
            cout << "Enter The Program Id .\n";
            cin >> programid;
            viewAllCourses(programid);
        }
        if (opt == 2)
        {
            system("cls");
            int programid;
            cout << "Enter The Program Id .\n";
            cin >> programid;
            addCourseToProgram(programid);
        }
        if (opt == 3) {
            system("cls");
            updateCourse();
            cout << endl;
        }
        if (opt == 4)
        {
            system("cls");
            int programID, courseID;
            cout << "Enter Program ID: ";
            cin >> programID;
            cout << "Enter Course ID: ";
            cin >> courseID;
            removeCourse(programID, courseID);
        }
        if (opt == 0)
        {
            system("cls");
            break;
        }

    } while (true);

}
void ProgramManagement()
{
    do
    {

        int opt;
        cout << "Choose Option: " << endl;
        cout << "1. View Program Details.\n";
        cout << "2. Add a new  Program.\n";
        cout << "3. Update Program Details.\n";
        cout << "4. Delete Program.\n";
        cout << "0. Exit.\n";
        cin >> opt;
        if (opt == 1)
        {
            system("cls");
            viewAllProgramsFromFile();
            cout << endl;
        }
        if (opt == 3)
        {
            system("cls");
            int programID;
            string newName, newDescription;
            cout << "Enter program ID to update: ";
            cin >> programID;
            cout << "Enter new program name: ";
            cin.ignore();
            getline(cin, newName);
            cout << "Enter new program description: ";
            getline(cin, newDescription);
            updateProgramInFile(programID, newName, newDescription);
            cout << endl;

        }
        if (opt == 4)
        {
            system("cls");
            int id;
            cout << "Entrer Program ID to remove.\n";
            cin >> id;
            removeProgramFromFile(id);
            cout << "Program deleted successfully.\n";
            cout << endl;
            break;
        }
        if (opt == 2)
        {
            system("cls");
            addProgramToFile();
            cout << endl;
        }
        if (opt == 0) {
            system("cls");
            cout << endl;
            break;
        }
    } while (true);
}
void AdminInterface()
{
    cout << "Enter Username: ";
    string usern;
    cin.ignore();
    getline(cin, usern);
    cout << "Enter Password: ";
    string pass;
    getline(cin, pass);
    ifstream adminFile("Admin.txt");
    if (!adminFile.is_open())
    {
        cerr << "Error opening Admin.txt" << endl;
        return;
    }
    string user;
    string password;
    getline(adminFile, user);
    getline(adminFile, password);
    adminFile.close();
    if (usern == user && pass == password)
    {
        cout << "Login successful. Welcome, Admin!" << endl;
        do
        {
            system("cls");
            int opt;
            cout << "Choose Option: " << endl;
            cout << "1. Program Management.\n";
            cout << "2. Manage Courses.\n";
            cout << "3. PLO Management.\n";
            cout << "4. CLO Management.\n";
            cout << "0. Exit.\n";
            cin >> opt;
            if (opt == 1)
            {
                system("cls");
                ProgramManagement();
                cout << endl;

            }
            if (opt == 2)
            {
                system("cls");
                CourseManagement();
                cout << endl;
            }
            if (opt == 3)
            {
                system("cls");
                PLOManagement();
                cout << endl;
            }
            if (opt == 4)
            {
                system("cls");
                CLOManagement();
                cout << endl;
            }
            if (opt == 0)
            {
                system("cls");
                cout << endl;
                break;
            }
        } while (true);
    }
    else
    {
        cout << "Incorrect username or password." << endl;
    }
}
// EVALUTION MANAGEMENT
void EvaulationManagement()
{
    EvaluationManager manager;
    manager.readFromFile("evaluations.txt");
    do
    {
        int opt;
        cout << "Choose Option: " << endl;
        cout << "1. View ALL Evalutions to specific Course.\n";
        cout << "2. Add Evalution to Specific Course.\n";
        cout << "3. Update Evalution to specific Course.\n";
        cout << "4. Delete Evalution to Specific Course.\n";
        cout << "0. PRESS to EXIT.\n ";
        cin >> opt;
        if (opt == 1)
        {
            system("cls");
            manager.viewAllEvaluations();
            cout << endl;
        }
        if (opt == 2)
        {
            system("cls");
            int evalutionid;
            int courseid;
            string type;
            cout << "Enter Course ID .\n";
            cin >> courseid;
            if (isCourseExists(courseid))
            {
                cout << "Enter Evalution ID.\n";
                cin >> evalutionid;
                cout << "Enter Evalution Type.\n";
                cin.ignore();
                getline(cin, type);
                manager.addEvaluation(evalutionid, type, courseid);
                cout << endl;
            }
            else
            {
                cout << "Enter Valid Course ID.\n";
                cout << endl;
            }
        }
        if (opt == 3)
        {
            system("cls");
            int evalutionid;
            int courseid;
            cout << "Enter Course ID.\n";
            cin >> courseid;

            string type;
            if (isCourseExists(courseid))
            {

                cout << "Enter Evalution ID.\n";
                cin >> evalutionid;
                cout << "Enter Evalution Type.\n";
                cin.ignore();
                getline(cin, type);
                manager.updateEvaluation(evalutionid, type, courseid);
                cout << endl;
            }



        }
        if (opt == 4)
        {
            system("cls");
            int evalutionid;
            cout << "Enter Evalution ID.\n";
            cin >> evalutionid;
            manager.deleteEvaluation(evalutionid);
            cout << endl;
        }
        manager.writeToFile("evaluations.txt");
        if (opt == 0)
        {
            system("cls");
            cout << endl;
            break;
        }
    } while (true);
}
bool isCLOTestedTwice(int cloId) {
    ifstream inFile("question.txt");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open file "<< endl;
        return false;
    }

    int count = 0;
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string token;
        int extractedCloId;

        // Skip over unnecessary data to reach the CLO ID
        getline(ss, token, ',');  // Question ID
        getline(ss, token, ',');  // Question description
        getline(ss, token, ',');  // Evaluation ID

        // Get the CLO ID
        getline(ss, token, ',');
        extractedCloId = stoi(token);

        // Count how many times the CLO ID appears
        if (extractedCloId == cloId) {
            count++;
        }

        // If it's tested at least twice, return true
        if (count >= 2) {
            inFile.close();
            return true;
        }
    }

    inFile.close();
    return false;
}
vector<int> getCLOsForCourse(int courseId, const string& cloFilename) {
    ifstream inFile(cloFilename);
    vector<int> courseCLOs;

    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string token;

            // Extract CLO data
            getline(ss, token, ',');  // CLO ID
            int cloId = stoi(token);

            getline(ss, token, ',');  // CLO description

            getline(ss, token, ',');  // Course ID
            int cloCourseId = stoi(token);

            // If the course ID matches, add the CLO ID to the list
            if (cloCourseId == courseId) {
                courseCLOs.push_back(cloId);
            }
        }
        inFile.close();
    }
    else {
        cerr << "Error: Unable to open file " << cloFilename << endl;
    }

    return courseCLOs;
}
// Function to check if all CLOs of a given course are tested

set<int> getRelevantCourseIds(int ploId, const string& cloFilename) {
    set<int> relevantCourseIds; // Unique course IDs for the specified PLO

    // Read CLOs to find course IDs linked to the given PLO
    ifstream cloFile(cloFilename);
    if (!cloFile.is_open()) {
        cerr << "Error: Unable to open file " << cloFilename << endl;
        return relevantCourseIds;
    }

    string line;
    while (getline(cloFile, line)) {
        stringstream ss(line);
        string token;

        getline(ss, token, ',');  // CLO ID
        getline(ss, token, ',');  // CLO description
        getline(ss, token, ',');  // Course ID
        int courseId = stoi(token);

        getline(ss, token, ',');  // PLO ID
        int cloPloId = stoi(token);

        // If the PLO ID matches, add the course ID to the set
        if (cloPloId == ploId) {
            relevantCourseIds.insert(courseId);
        }
    }

    cloFile.close();

    return relevantCourseIds;
}

// Function to get course details from course.txt based on course IDs
map<int, string> getCourseDetails(const set<int>& courseIds, const string& courseFilename) {
    map<int, string> courseDetails; // Maps course IDs to course names

    ifstream courseFile(courseFilename);
    if (!courseFile.is_open()) {
        cerr << "Error: Unable to open file " << courseFilename << endl;
        return courseDetails;
    }

    string line;
    while (getline(courseFile, line)) {
        stringstream ss(line);
        string token;

        getline(ss, token, ',');  // Program ID
        getline(ss, token, ',');  // Course ID
        int courseId = stoi(token);

        getline(ss, token, ',');  // Course name
        string courseName = token;

        getline(ss, token, ',');  // Course description

        // If the course ID is in the set, add its name to the map
        if (courseIds.find(courseId) != courseIds.end()) {
            courseDetails[courseId] = courseName;
        }
    }

    courseFile.close();

    return courseDetails;
}

bool areAllCLOsTested(int courseId, const vector<int>& courseCLOs, const string& questionFilename) {
    ifstream inFile(questionFilename);
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open file " << questionFilename << endl;
        return false;
    }

    vector<bool> cloTested(courseCLOs.size(), false); // Array to track which CLOs are tested
    string line;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string token;

        getline(ss, token, ',');  // Question ID
        getline(ss, token, ',');  // Question description
        getline(ss, token, ',');  // Evaluation ID

        getline(ss, token, ',');  // CLO ID
        int cloId = stoi(token);

        // Check if this CLO is in the list of course CLOs
        for (size_t i = 0; i < courseCLOs.size(); ++i) {
            if (courseCLOs[i] == cloId) {
                cloTested[i] = true; // Mark as tested
                break;
            }
        }
    }

    inFile.close();

    // Return true if all CLOs are tested
    for (size_t i = 0; i < courseCLOs.size(); ++i) {
        if (!cloTested[i]) {
            return false; // If any CLO is not tested, return false
        }
    }

    return true; // All CLOs are tested
}
void Questionmanagement(const string& filename) {
    int choice;
    do
    {
        cout << "\nMenu:\n";
        cout << "1. View all questions\n";
        cout << "2. Add a question\n";
        cout << "3. Update a question\n";
        cout << "4. Delete a question\n";
        cout << "5. CLO has been tested in two different questions\n";
        cout << "6. Check if all CLOs of a given course have been tested\n";
        cout << "7. Generate a list of all relevant courses for a given PLO\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            viewAllQuestions(filename);
            cout << endl;
            break;
        case 2:
        {
            int id, evaluationId;
            string text, clo;
            cout << "Enter ID: ";
            cin >> id;
            cout << "Enter text: ";
            cin.ignore();
            getline(cin, text);
            cout << "Enter evaluation ID: ";
            cin >> evaluationId;
            cout << "Enter CLO: ";
            cin.ignore();
            getline(cin, clo);
            addQuestion(id, text, evaluationId, clo, filename);
            cout << endl;
            break;
        }
        case 3:
        {
            int id;
            string newText, newCLO;
            cout << "Enter ID of the question to update: ";
            cin >> id;
            cout << "Enter new text: ";
            cin.ignore();
            getline(cin, newText);
            cout << "Enter new CLO: ";
            getline(cin, newCLO);
            updateQuestion(id, newText, newCLO, filename);
            cout << endl;
            break;
        }
        case 4:
        {
            int id;
            cout << "Enter ID of the question to delete: ";
            cin >> id;
            deleteQuestion(id, filename);
            cout << endl;
            break;
        }
        case 5:
        {
            int cloIdToCheck;
            cout << "Enter CLO ID to be tested.\n";
            cin >> cloIdToCheck;
            bool testedTwice = isCLOTestedTwice(cloIdToCheck);
            if (testedTwice) {
                cout << "CLO " << cloIdToCheck << " has been tested in at least two different questions." << endl;
            }
            else {
                cout << "CLO " << cloIdToCheck << " has not been tested in at least two different questions." << endl;
            }
            break;
        }
        case 6:
        {
            int courseIdToCheck,programid;
            cout << "Enter Program ID .\n";
            cin >> programid;
            
            cout << "Enter Course id CLOS to be tested .\n";
            cin >> courseIdToCheck;

            if (courseExists(programid,courseIdToCheck))
            {
                string cloFilename = "clos.txt"; // File containing CLO data
                string questionFilename = "question.txt"; // File containing question data

                // Get the CLOs associated with the specified course
                    vector<int> courseCLOs = getCLOsForCourse(courseIdToCheck, cloFilename);

                    // Check if all CLOs for this course have been tested
                    bool result = areAllCLOsTested(courseIdToCheck, courseCLOs, questionFilename);

                    cout << "Are all CLOs for course ID " << courseIdToCheck << " tested? " << (result ? "Yes" : "No") << endl;
                break;
            }

            else
            {
                cout << "Course ID is INVALID.\n";
                break;
            }

            

        }
        case 7:
        {
            int ploId;
            cout << "Enter PLO ID for relevant Courses.\n";
            cin >> ploId;
            string cloFilename = "clos.txt";  // File with CLO data
            string courseFilename = "courses.txt";  // File with course data

            // Get the relevant course IDs for the given PLO
            set<int> relevantCourseIds = getRelevantCourseIds(ploId, cloFilename);

            // Get the details of these courses (ID and name)
            map<int, string> courseDetails = getCourseDetails(relevantCourseIds, courseFilename);

            cout << "Courses relevant to PLO " << ploId << ":" << endl;
            for (const auto& entry : courseDetails) {
                cout << "Course ID: " << entry.first << ", Course Name: " << entry.second << endl;
            }
            break;
        }
        case 0:
            cout << "Exiting...\n";
            cout << endl;
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
        break;
    }

    while (choice != 7);

}
void TeacherInterface()
{

    cout << "Enter Teacher ID: ";
    string usern;
    cin.ignore();
    getline(cin, usern);
    cout << "Enter Password: ";
    string pass;
    getline(cin, pass);
    ifstream File("teacher.txt");
    if (!File.is_open())
    {
        cerr << "Error opening Teacher.txt" << endl;
        return;
    }
    string user;
    string password;
    getline(File, user);
    getline(File, password);
    File.close();
    if (usern == user && pass == password)
    {
        cout << "Login successful. Welcome, Teacher!" << endl;
        do
        {

            int opt;
            cout << "Choose Option: " << endl;
            cout << " 1. Evaluation Management.\n";
            cout << " 2. Question Management\n";
            cout << "5. Reporting and Analysis.\n";
            cout << "0. Exit.\n";
            cin >> opt;
            if (opt == 1)
            {
                system("cls");
                EvaulationManagement();
                cout << endl;
            }
            if (opt == 2)
            {
                system("cls");
                string filename;
                filename = "question.txt";
                Questionmanagement(filename);
                cout << endl;
            }
            if (opt == 0)
            {
                system("cls");
                break;
            }

        } while (true);
    }
    else
    {
        cout << "Incorrect username or password." << endl;
    }
}
int main() {
    do {
        system("cls");
        cout << "OBE Support System.\n";
        cout << "Select option: \n";
        cout << "1. Teacher\n";
        cout << "2. Admin\n";
        cout << "Enter: ";
        int option;
        cin >> option;
        if (option == 1) {
            system("cls");
            TeacherInterface();
        }
        else if (option == 2) {
            system("cls");
            AdminInterface();
        }
        else if (option == 0) {
            break;
        }

    } while (true);
    return 0;
}
