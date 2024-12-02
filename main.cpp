#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

class Student {
public:
    std::string name;
    std::vector<std::string> preferences;

    // Default constructor
    Student() = default;

    Student(const std::string& studentName) : name(studentName) {}

    void addPreference(const std::string& sectionName) {
        preferences.push_back(sectionName);
    }
};

class Section {
public:
    std::string name;
    std::unordered_set<std::string> students;

    // Default constructor
    Section() = default;

    Section(const std::string& sectionName) : name(sectionName) {}

    void addStudent(const std::string& studentName) {
        students.insert(studentName);
    }
};

class StudentSectionGraph {
private:
    std::unordered_map<std::string, Student> students;
    std::unordered_map<std::string, Section> sections;

    // Helper method to find the section a student is currently in
    std::string findStudentSection(const std::string& studentName) const {
        for (const auto& [sectionName, section] : sections) {
            if (section.students.count(studentName) > 0) {
                return sectionName;
            }
        }
        return "";
    }

public:
    void initializeGraph() {
        // Create sections 
        sections.emplace("history", Section("history"));
        sections.emplace("math", Section("math"));
        sections.emplace("science", Section("science"));

        // Add initial students to sections
        sections["history"].addStudent("A");
        sections["history"].addStudent("B");
        sections["math"].addStudent("P");
        sections["math"].addStudent("Q");
        sections["science"].addStudent("R");
        sections["science"].addStudent("S");

        // Create students 
        students.emplace("A", Student("A"));
        students.emplace("B", Student("B"));
        students.emplace("P", Student("P"));
        students.emplace("Q", Student("Q"));
        students.emplace("R", Student("R"));
        students.emplace("S", Student("S"));

        // Add student preferences for cycle testing
        // Example cycles
        students["A"].addPreference("math");
        students["P"].addPreference("science");
        students["S"].addPreference("history");
    }

    int countUniqueCyclesForStudent(const std::string& studentId) {
        if (students.find(studentId) == students.end()) {
            std::cout << "Student " << studentId << " not found.\n";
            return 0;
        }

        std::unordered_set<std::string> uniqueCycles;

        // Depth-first search to find cycles
        std::unordered_set<std::string> visited;
        std::vector<std::string> path;

        auto findCycles = [&](auto& self, const std::string& currentStudent) -> void {
            // Mark current student as visited
            visited.insert(currentStudent);
            path.push_back(currentStudent);

            // Get current student's current section
            std::string currentSection = findStudentSection(currentStudent);
            
            // Try all students
            for (const auto& [otherStudent, student] : students) {
                // Skip if same student or already visited
                if (otherStudent == currentStudent || 
                    visited.count(otherStudent) > 0) continue;

                // Get other student's current section
                std::string otherSection = findStudentSection(otherStudent);

                // Check if other student wants current student's section
                bool otherWantsCurrentSection = 
                    std::find(student.preferences.begin(), 
                              student.preferences.end(), 
                              currentSection) != student.preferences.end();

                // Check if current student wants other student's section
                bool currentWantsOtherSection = 
                    std::find(students.at(currentStudent).preferences.begin(), 
                              students.at(currentStudent).preferences.end(), 
                              otherSection) != students.at(currentStudent).preferences.end();

                // If mutual section swap desire exists
                if (currentWantsOtherSection && otherWantsCurrentSection) {
                    // Recursive DFS to continue the path
                    path.push_back(otherStudent);
                    
                    // Check if this creates a cycle back to original student
                    if (otherStudent == studentId) {
                        // Found a cycle
                        std::vector<std::string> cycle = path;
                        std::sort(cycle.begin(), cycle.end());
                        
                        // Create unique cycle key
                        std::string cycleKey;
                        for (const auto& s : cycle) {
                            cycleKey += s + ",";
                        }
                        uniqueCycles.insert(cycleKey);
                    } else {
                        // Continue DFS
                        self(self, otherStudent);
                    }
                    
                    // Backtrack
                    path.pop_back();
                }
            }

            // Backtrack
            visited.erase(currentStudent);
            path.pop_back();
        };

        // Start cycle detection from the given student
        findCycles(findCycles, studentId);

        return uniqueCycles.size();
    }

    void printGraph() {
        std::cout << "Graph Structure:\n";
        
        // Print Students and their preferences
        std::cout << "\nStudents and their Preferences:\n";
        for (const auto& [studentName, student] : students) {
            std::cout << studentName << " prefers: ";
            for (const auto& pref : student.preferences) {
                std::cout << pref << " ";
            }
            std::cout << "\n";
        }
        
        // Print Sections and their students
        std::cout << "\nSections and their Students:\n";
        for (const auto& [sectionName, section] : sections) {
            std::cout << sectionName << ": ";
            for (const auto& studentName : section.students) {
                std::cout << studentName << " ";
            }
            std::cout << "\n";
        }
    }
};

int main() {
    StudentSectionGraph graph;
    
    // Initialize the graph with the given configuration
    graph.initializeGraph();
    
    // Print initial graph state
    std::cout << "Initial Graph State:\n";
    graph.printGraph();

    // Count unique cycles for specific students
    std::cout << "\nUnique Cycles for Student A: " 
              << graph.countUniqueCyclesForStudent("A") << std::endl;
    std::cout << "Unique Cycles for Student B: " 
              << graph.countUniqueCyclesForStudent("B") << std::endl;
    std::cout << "Unique Cycles for Student P: " 
              << graph.countUniqueCyclesForStudent("P") << std::endl;
    std::cout << "Unique Cycles for Student Q: " 
              << graph.countUniqueCyclesForStudent("Q") << std::endl;
    std::cout << "Unique Cycles for Student R: " 
              << graph.countUniqueCyclesForStudent("R") << std::endl;
    std::cout << "Unique Cycles for Student S: " 
              << graph.countUniqueCyclesForStudent("S") << std::endl;

    return 0;
}