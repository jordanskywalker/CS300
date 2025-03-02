//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Jordan Walker Feb 22, 2025
// Version     : 1.0
// Copyright   : Your copyright notice
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm> // For transform (case normalization)
#include <limits>    // For numeric_limits

using namespace std;

// Course structure to hold course details
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;

    Course() {}
    Course(string num, string name, vector<string> prereqs)
        : courseNumber(num), courseName(name), prerequisites(prereqs) {}
};

// BST Node structure
struct BSTNode {
    Course course;
    BSTNode* left;
    BSTNode* right;

    BSTNode(Course c) : course(c), left(nullptr), right(nullptr) {}
};

// Binary Search Tree class
class BinarySearchTree {
private:
    BSTNode* root;

    // Recursive helper function for inserting a node
    BSTNode* insertRecursive(BSTNode* node, Course course) {
        if (node == nullptr) {
            return new BSTNode(course);
        }
        if (course.courseNumber < node->course.courseNumber) {
            node->left = insertRecursive(node->left, course);
        } else {
            node->right = insertRecursive(node->right, course);
        }
        return node;
    }

    // Recursive helper function for in-order traversal
    void inOrderTraversal(BSTNode* node) {
        if (node != nullptr) {
            inOrderTraversal(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseName << endl;
            inOrderTraversal(node->right);
        }
    }

    // Recursive helper function for searching a course
    BSTNode* searchRecursive(BSTNode* node, string courseNumber) {
        if (node == nullptr || node->course.courseNumber == courseNumber) {
            return node;
        }
        if (courseNumber < node->course.courseNumber) {
            return searchRecursive(node->left, courseNumber);
        }
        return searchRecursive(node->right, courseNumber);
    }

    // Recursive helper function for removing a course
    BSTNode* removeRecursive(BSTNode* node, string courseNumber, bool& found) {
        if (node == nullptr) {
            found = false;
            return nullptr;
        }

        if (courseNumber < node->course.courseNumber) {
            node->left = removeRecursive(node->left, courseNumber, found);
        } else if (courseNumber > node->course.courseNumber) {
            node->right = removeRecursive(node->right, courseNumber, found);
        } else {
            found = true;
            // Case 1: No children
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                return nullptr;
            }
            // Case 2: One child
            if (node->left == nullptr) {
                BSTNode* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                BSTNode* temp = node->left;
                delete node;
                return temp;
            }
            // Case 3: Two children
            BSTNode* successor = findMin(node->right);
            node->course = successor->course;
            node->right = removeRecursive(node->right, successor->course.courseNumber, found);
        }
        return node;
    }

    // Helper function to find the minimum node (successor)
    BSTNode* findMin(BSTNode* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    // Recursive helper function to destroy the tree
    void destroyTree(BSTNode* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() : root(nullptr) {}

    // Destructor to prevent memory leaks
    ~BinarySearchTree() {
        destroyTree(root);
    }

    void insert(Course course) {
        root = insertRecursive(root, course);
    }

    void displayCourses() {
        if (root == nullptr) {
            cout << "No courses available." << endl;
            return;
        }
        cout << "Here is a sample schedule:" << endl;
        inOrderTraversal(root);
    }

    void displayCourseInfo(string courseNumber) {
        transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);
        BSTNode* found = searchRecursive(root, courseNumber);
        if (found != nullptr) {
            cout << found->course.courseNumber << ", " << found->course.courseName << endl;
            if (!found->course.prerequisites.empty()) {
                cout << "Prerequisites: ";
                for (size_t i = 0; i < found->course.prerequisites.size(); i++) {
                    cout << found->course.prerequisites[i];
                    if (i != found->course.prerequisites.size() - 1) cout << ", ";
                }
                cout << endl;
            } else {
                cout << "Prerequisites: None" << endl;
            }
        } else {
            cout << "Hmmm... couldn't find that course." << endl;
        }
    }

    void removeCourse(string courseNumber) {
        transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);
        bool found = false;
        root = removeRecursive(root, courseNumber, found);
        cout << (found ? "Course removed successfully!" : "Course not found.") << endl;
    }
};

// Function to load courses from a CSV file
BinarySearchTree loadCoursesFromFile(string filename) {
    BinarySearchTree bst;
    ifstream file(filename);
    if (!file) {
        cout << "Oops! Couldn't open the file '" << filename << "'." << endl;
        return bst;
    }

    string line;
    int lineNum = 0;
    while (getline(file, line)) {
        lineNum++;
        stringstream ss(line);
        vector<string> tokens;
        string token;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 2) {
            cout << "Malformed line " << lineNum << ": '" << line << "'" << endl;
            continue;
        }

        string courseNumber = tokens[0];
        string courseName = tokens[1];
        vector<string> prerequisites;
        for (size_t i = 2; i < tokens.size(); i++) {
            prerequisites.push_back(tokens[i]);
        }

        transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);
        Course course(courseNumber, courseName, prerequisites);
        bst.insert(course);
    }

    file.close();
    return bst;
}

int main() {
    BinarySearchTree bst;
    bool dataLoaded = false;
    string filename = "CS 300 ABCU_Advising_Program_Input.csv"; // Update this path as needed

    int choice;
    while (true) {
        cout << "\nWelcome to the course planner.\n";
        cout << "1. Load Data Structure\n";
        cout << "2. Print Course List\n";
        cout << "3. Print Course\n";
        cout << "4. Remove Course\n";
        cout << "9. Exit\n";
        cout << "\nWhat would you like to do? ";

        if (!(cin >> choice)) {
            cout << "Please enter a valid number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer for getline

        switch (choice) {
            case 1:
                bst = loadCoursesFromFile(filename);
                dataLoaded = true;
                cout << "Course data loaded successfully!" << endl;
                break;
            case 2:
                if (dataLoaded) {
                    bst.displayCourses();
                } else {
                    cout << "Please load the data first!" << endl;
                }
                break;
            case 3:
                if (dataLoaded) {
                    cout << "What course do you want to know about? ";
                    string courseNum;
                    getline(cin, courseNum);
                    if (courseNum.empty()) {
                        cout << "Please enter a course number." << endl;
                        break;
                    }
                    bst.displayCourseInfo(courseNum);
                } else {
                    cout << "Please load the data first!" << endl;
                }
                break;
            case 4:
                if (dataLoaded) {
                    cout << "What course do you want to remove? ";
                    string courseNum;
                    getline(cin, courseNum);
                    if (courseNum.empty()) {
                        cout << "Please enter a course number." << endl;
                        break;
                    }
                    bst.removeCourse(courseNum);
                } else {
                    cout << "Please load the data first!" << endl;
                }
                break;
            case 9:
                cout << "Thank you for using the course planner!" << endl;
                return 0;
            default:
                cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}