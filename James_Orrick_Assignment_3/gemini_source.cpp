/*
gemini_source.cpp
Desc: a C++ program that takes commands from an input file and sorts them using a Maxheap
Author: James Orrick
Created 10/1/2026
Human Collaborators: None
AIs involved: Gemini,Copilot
*/



#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// ============================================================================
// Email Class
// Represents an individual email object with priority and date comparison logic
// ============================================================================
class Email {
private:
    std::string senderCategory;
    std::string subject;
    std::string dateStr;
    int priorityRank; // Lower numerical value = higher priority
    int year;
    int month;
    int day;

    // ------------------------------------------------------------------------
    // Parses date string formatted as "MM-DD-YYYY" into numeric fields
    // ------------------------------------------------------------------------
    void parseDate(const std::string& date) {
        // Expected format: MM-DD-YYYY
        if (date.length() >= 10) {
            month = std::stoi(date.substr(0, 2));
            day = std::stoi(date.substr(3, 2));
            year = std::stoi(date.substr(6, 4));
        } else {
            year = month = day = 0;
        }
    }

    // ------------------------------------------------------------------------
    // Maps sender category string to a numerical priority rank
    // ------------------------------------------------------------------------
    void assignPriorityRank(const std::string& category) {
        if (category == "Boss") priorityRank = 1;
        else if (category == "Subordinate") priorityRank = 2;
        else if (category == "Peer") priorityRank = 3;
        else if (category == "ImportantPerson") priorityRank = 4;
        else priorityRank = 5; // "OtherPerson"
    }

public:
    // Default constructor
    Email() : priorityRank(5), year(0), month(0), day(0) {}

    // Parameterized constructor
    Email(const std::string& category, const std::string& subj, const std::string& date)
        : senderCategory(category), subject(subj), dateStr(date) {
        assignPriorityRank(category);
        parseDate(date);
    }

    // Getters
    std::string getSenderCategory() const { return senderCategory; }
    std::string getSubject() const { return subject; }
    std::string getDateStr() const { return dateStr; }

   // ------------------------------------------------------------------------
    // Priority comparison: Returns true if *this has HIGHER priority than 'other'
    // ------------------------------------------------------------------------
    bool hasHigherPriorityThan(const Email& other) const {
        // Lower priority rank number wins
        if (this->priorityRank != other.priorityRank) {
            return this->priorityRank < other.priorityRank;
        }
        // If same category, newest email first (larger date)
        if (this->year != other.year) return this->year > other.year;
        if (this->month != other.month) return this->month > other.month;
        return this->day > other.day;
    }
};

// ============================================================================
// HeapNode Class
// Represents a single node in a binary tree implementation of a MaxHeap
// ============================================================================
class HeapNode {
public:
    Email data;
    HeapNode* left;
    HeapNode* right;
    HeapNode* parent;

    HeapNode(const Email& email) 
        : data(email), left(nullptr), right(nullptr), parent(nullptr) {}
};

// ============================================================================
//  MaxHeap Class 
// List/Pointer-based MaxHeap Priority Queue implementation for Email objects
// ============================================================================
class MaxHeap {
private:
    HeapNode* root;
    int count;

    // ------------------------------------------------------------------------
    // Locates a node using its 1-based level-order binary index path
    // ------------------------------------------------------------------------
    HeapNode* getNodeAt(int index) const {
        if (index <= 0 || index > count) return nullptr;
        if (index == 1) return root;

        // Path finding using binary representation of the index
        int path[32];
        int pathLen = 0;
        int temp = index;

        while (temp > 1) {
            path[pathLen++] = temp % 2; // 0 for left, 1 for right
            temp /= 2;
        }

        HeapNode* curr = root;
        for (int i = pathLen - 1; i >= 0; --i) {
            if (path[i] == 0) curr = curr->left;
            else curr = curr->right;
        }
        return curr;
    }
    // ------------------------------------------------------------------------
    // Restores heap property upward from a target node
    // ------------------------------------------------------------------------
    void bubbleUp(HeapNode* node) {
        while (node->parent != nullptr && node->data.hasHigherPriorityThan(node->parent->data)) {
            // Swap email data between node and parent
            Email temp = node->data;
            node->data = node->parent->data;
            node->parent->data = temp;

            node = node->parent;
        }
    }
    // ------------------------------------------------------------------------
    // Restores heap property downward from a target node
    // ------------------------------------------------------------------------
    void bubbleDown(HeapNode* node) {
        while (node != nullptr) {
            HeapNode* highest = node;

            if (node->left && node->left->data.hasHigherPriorityThan(highest->data)) {
                highest = node->left;
            }
            if (node->right && node->right->data.hasHigherPriorityThan(highest->data)) {
                highest = node->right;
            }

            if (highest != node) {
                Email temp = node->data;
                node->data = highest->data;
                highest->data = temp;

                node = highest;
            } else {
                break;
            }
        }
    }
    // ------------------------------------------------------------------------
    // Recursively deletes all nodes in post-order traversal
    // ------------------------------------------------------------------------
    void destroyTree(HeapNode* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    MaxHeap() : root(nullptr), count(0) {}

    ~MaxHeap() {
        destroyTree(root);
    }

    bool isEmpty() const {
        return count == 0;
    }

    int getCount() const {
        return count;
    }
    // ------------------------------------------------------------------------
    // Inserts a new email into the heap and restores heap order
    // ------------------------------------------------------------------------
    void insert(const Email& email) {
        count++;
        HeapNode* newNode = new HeapNode(email);

        if (count == 1) {
            root = newNode;
            return;
        }

        // Find parent position for newly added node at index 'count'
        HeapNode* parentNode = getNodeAt(count / 2);
        newNode->parent = parentNode;

        if (count % 2 == 0) {
            parentNode->left = newNode;
        } else {
            parentNode->right = newNode;
        }

        bubbleUp(newNode);
    }

    // ------------------------------------------------------------------------
    // Returns the highest-priority email at the root without removing it
    // ------------------------------------------------------------------------
    Email peekMax() const {
        if (!isEmpty()) {
            return root->data;
        }
        return Email();
    }

    // ------------------------------------------------------------------------
    // Removes the root email from heap and restores heap structure
    // ------------------------------------------------------------------------
    void extractMax() {
        if (isEmpty()) return;

        if (count == 1) {
            delete root;
            root = nullptr;
            count = 0;
            return;
        }

        HeapNode* lastNode = getNodeAt(count);
        root->data = lastNode->data; // Move last element to root

        // Detach last node from parent
        HeapNode* parentNode = lastNode->parent;
        if (parentNode->left == lastNode) {
            parentNode->left = nullptr;
        } else {
            parentNode->right = nullptr;
        }

        delete lastNode;
        count--;

        bubbleDown(root);
    }
};

// ============================================================================
// CLASS: EmailManager
// Parses command input stream and manages operations on the MaxHeap
// ============================================================================
class EmailManager {
private:
    MaxHeap priorityQueue;

    // ------------------------------------------------------------------------
    // Helper: Trims leading and trailing whitespace from string
    // ------------------------------------------------------------------------
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }

public:
    // ------------------------------------------------------------------------
    // Parses and executes an individual command string
    // ------------------------------------------------------------------------
    void processCommand(const std::string& line) {
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty()) return;

        // Command: EMAIL <Category>, <Subject>, <Date>
        if (trimmedLine.rfind("EMAIL ", 0) == 0) {
            std::string data = trimmedLine.substr(6);
            std::stringstream ss(data);
            std::string category, subject, date;

            if (std::getline(ss, category, ',') &&
                std::getline(ss, subject, ',') &&
                std::getline(ss, date, ',')) {
                
                Email email(trim(category), trim(subject), trim(date));
                priorityQueue.insert(email);
            }
        }
        // Command: COUNT
        else if (trimmedLine == "COUNT") {
            std::cout << "There are " << priorityQueue.getCount() << " emails to read.\n\n";
        } 
        // Command: NEXT
        else if (trimmedLine == "NEXT") {
            if (!priorityQueue.isEmpty()) {
                Email top = priorityQueue.peekMax();
                std::cout << "Next email:\n";
                std::cout << "Sender: " << top.getSenderCategory() << "\n";
                std::cout << "Subject: " << top.getSubject() << "\n";
                std::cout << "Date: " << top.getDateStr() << "\n\n";
            }
        } 
        // Command: READ
        else if (trimmedLine == "READ") {
            if (!priorityQueue.isEmpty()) {
                priorityQueue.extractMax();
            }
        }
    }
    // ------------------------------------------------------------------------
    // Reads input line by line from an input stream
    // ------------------------------------------------------------------------
    void processStream(std::istream& in) {
        std::string line;
        while (std::getline(in, line)) {
            processCommand(line);
        }
    }
};

// ============================================================================
// -------MAIN---------
// ============================================================================

int main(int argc, char* argv[]) {
    EmailManager manager;

    if (argc > 1) {
        
        // Process from command line argument file if provided
        std::ifstream file(argv[1]);
        if (file.is_open()) {
            manager.processStream(file);
            file.close();
        } else {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            return 1;
        }
    } else {
        // Fallback to reading from standard input
        manager.processStream(std::cin);
    }

    return 0;
}
