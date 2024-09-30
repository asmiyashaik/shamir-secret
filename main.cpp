#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <vector>
#include <string>
#include "json.hpp" 
using json = nlohmann::json;
using namespace std;

// Structure to store a root (x, y)
struct Root {
    int x;        // x-coordinate
    long long y;  // y-coordinate (after decoding)
};

// Function to convert a value from any base to decimal
long long decodeValue(const string& value, int base) {
    long long result = 0;
    int len = value.length();
    for (int i = 0; i < len; i++) {
        char digit = value[i];
        int num;
        if (isdigit(digit)) {
            num = digit - '0';
        } else {
            num = toupper(digit) - 'A' + 10; 
        }
        result = result * base + num;
    }
    return result;
}

// Function to read and parse the JSON data from the file
void readJsonData(const string& fileName, map<int, Root>& roots, int& n, int& k) {
    ifstream file(fileName);
    json j;
    file >> j;
    n = j["keys"]["n"]; // Total number of points provided
    k = j["keys"]["k"]; // Minimum number of points needed (k = m + 1)

    for (auto& item : j.items()) {
        if (item.key() != "keys") {
            int x = stoi(item.key()); // x is the key in the JSON object
            int base = stoi(item.value()["base"].get<string>()); // Base of the y-value
            string y_value = item.value()["value"].get<string>(); // Encoded y-value as a string
            long long y = decodeValue(y_value, base); // Decode y-value to decimal
            roots[x] = {x, y}; // Store x and decoded y in the map
        }
    }
}

// Function to compute Lagrange Interpolation to find the constant term (c)
double lagrangeInterpolation(const map<int, Root>& roots, int k, int x_val) {
    double result = 0.0;

    for (auto it_i = roots.begin(); it_i != next(roots.begin(), k); ++it_i) {
        const Root& root_i = it_i->second;
        double term = root_i.y;

        for (auto it_j = roots.begin(); it_j != next(roots.begin(), k); ++it_j) {
            if (it_i != it_j) {
                const Root& root_j = it_j->second;
                term *= (x_val - root_j.x) / (double)(root_i.x - root_j.x);
            }
        }
        result += term;
    }
    return result; 
}

// Function to find the constant term c for the polynomial
double findConstantTerm(const map<int, Root>& roots, int k) {
    // Calculate the constant term (c) at x = 0
    return lagrangeInterpolation(roots, k, 0);
}

// Function to find wrong points in Test Case 2 (points that don't lie on the polynomial)
vector<int> findWrongPoints(const map<int, Root>& roots, int k) {
    vector<int> wrongPoints;

    for (const auto& pair : roots) {
        int x = pair.first;
        long long actualY = pair.second.y;

        double expectedY = lagrangeInterpolation(roots, k, x);

        // Check if the difference between actual and expected y-values exceeds a threshold
        if (fabs(expectedY - actualY) > 1e-6) {
            wrongPoints.push_back(x);  
        }
    }

    return wrongPoints;  // Return the list of wrong points
}

// Main Function
int main() {
    map<int, Root> roots;
    int n, k;

    // **Test Case 1:**
    readJsonData("testcase1.json", roots, n, k); // Read the first JSON file

    // Calculate the constant term using Lagrange Interpolation for Test Case 1
    double constantTerm1 = findConstantTerm(roots, k);
    cout << "Constant term (Test Case 1): " << constantTerm1 << endl;

    // Clear the map for Test Case 2
    roots.clear();

    // **Test Case 2:**
    readJsonData("testcase2.json", roots, n, k); // Read the second JSON file

    // Calculate the constant term using Lagrange Interpolation for Test Case 2
    double constantTerm2 = findConstantTerm(roots, k);
    cout << "Constant term (Test Case 2): " << constantTerm2 << endl;

    // Find wrong points in Test Case 2
    vector<int> wrongPoints = findWrongPoints(roots, k);
    cout << "Wrong points in Test Case 2: ";
    if (wrongPoints.empty()) {
        cout << "None"; 
    } else {
        for (int wp : wrongPoints) {
            cout << wp << " ";
        }
    }
    cout << endl;

    return 0;
}
