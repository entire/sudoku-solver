#ifndef SUDOKUMETADATA_H
#define SUDOKUMETADATA_H

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>
#include <set>

namespace Sudoku {


struct Cell {
public:
   // this is the key of the cell, e.g. "A2", "C3", etc
    std::string key;
    // this is the value of the cell
    int value;
    // vector of units
    std::vector<std::vector<std::string>> units;
    // all possible cell values for this cell
    std::vector<int> candidates;
    // peers
    std::set<std::string> peers;
 
    // remove by target
    void removeCandidate(int& target) {
        // if (candidates.size() == 1) {
        //     return;
        // }
        // std::cout << "key: " << this->key << " - removeCandidate: " << target << " - cand size: " << candidates.size() << std::endl;
        candidates.erase(
            std::remove_if(candidates.begin(), candidates.end(),
            [&](int& s) { return s == target; }),
        candidates.end());
        // std::cout << "post remove canditate size: " << candidates.size() << std::endl;
   }

    // check if it has string
    bool hasCandidate(int& s) {
        return std::find(candidates.begin(), candidates.end(), s) != candidates.end();
    }

    // print the value of the cell
    void printValue() {
        if (value == 0) {
            std::cout << "value at " << key << " is " << value << std::endl;
        } else {
            std::cout << "value is empty or has not been initialized." << std::endl;
        }
    }
    // print all possible candidates
    void printCandidtes() {
        std::cout << "candidate for: " << key << std::endl;
        std::cout << "[ ";
        for (auto& p : candidates) {
            std::cout << p << " ";
        }
        std::cout << "]" << std::endl;
    }
    // print all units associated with cell
    void printUnits() {
        std::cout << "units for: " << key << std::endl;
        for (auto& unit : units) {
            std::cout << "[ ";
            for (auto& v : unit) {
                std::cout << v << " ";
            }
            std::cout << "]" << std::endl;
        }
    }
    // print all peers associated with cell
    void printPeers() {
        std::cout << "peers for: " << key << std::endl;
        std::cout << "[ ";
        for (auto& peer : peers) {
            std::cout << peer << " ";
        }
        std::cout << "]" << std::endl;
    }
};

struct Metadata {
public:
    std::vector<int> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<std::string> rows = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
    std::vector<std::string> cols = getStringsFromInts(digits);
    std::vector<std::string> squares = cross(rows, cols);
    std::unordered_map<std::string, Cell> cells;

    // default constructor
    Metadata();
    // cross two string vectors
    std::vector<std::string> cross(std::vector<std::string>& A, std::vector<std::string>& B);
    // get strings vector from ints
    static std::vector<std::string> getStringsFromInts(std::vector<int>& ints);
    // setting up unitlist
    void setupUnitlist(std::vector<std::vector<std::string>>& unitlist);
    // setup cells    
    void setupCells(std::unordered_map<std::string, Cell>& cells, std::vector<std::vector<std::string>>& unitlist);
    // setup peers
    void setupPeers(std::unordered_map<std::string, Cell>& cells);
    // destructor
    ~Metadata() = default;
};


} // namespace Sudoku

#endif /* SUDOKUMETADATA_H */