#ifndef SUDOKUMETAVARIABLE_H
#define SUDOKUMETAVARIABLE_H

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
    void RemoveCandidate(int& target) {
        candidates.erase(
            std::remove_if(candidates.begin(), candidates.end(),
            [&](int& s) { return s == target; }),
        candidates.end());
    }
    // check if the candidate has number
    bool HasCandidate(int& s) {
        return std::find(candidates.begin(), candidates.end(), s) != candidates.end();
    }

    // print the value of the cell
    void PrintValue() {
        if (value == 0) {
            std::cout << "value at " << key << " is " << value << std::endl;
        } else {
            std::cout << "value is empty or has not been initialized." << std::endl;
        }
    }
    // print all possible candidates
    void PrintCandidtes() {
        std::cout << "candidate for: " << key << std::endl;
        std::cout << "[ ";
        for (auto& p : candidates) {
            std::cout << p << " ";
        }
        std::cout << "]" << std::endl;
    }
    // print all units associated with cell
    void PrintUnits() {
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
    void PrintPeers() {
        std::cout << "peers for: " << key << std::endl;
        std::cout << "[ ";
        for (auto& peer : peers) {
            std::cout << peer << " ";
        }
        std::cout << "]" << std::endl;
    }
};

struct Variables {
public:
    std::vector<int> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<std::string> rows = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
    std::vector<std::string> cols = GetStringsFromInts(digits);
    std::vector<std::string> squares = Cross(rows, cols);
    std::unordered_map<std::string, Cell> cells;

    // default constructor
    Variables();
    // cross two string vectors
    std::vector<std::string> Cross(std::vector<std::string>& A, std::vector<std::string>& B);
    // get strings vector from ints
    static std::vector<std::string> GetStringsFromInts(std::vector<int>& ints);
    // setting up unitlist
    void SetupUnitlist(std::vector<std::vector<std::string>>& unitlist);
    // setup cells    
    void SetupCells(std::unordered_map<std::string, Cell>& cells, std::vector<std::vector<std::string>>& unitlist);
    // setup peers
    void SetupPeers(std::unordered_map<std::string, Cell>& cells);
    // destructor
    ~Variables() = default;
};

} // namespace Sudoku

#endif /* SUDOKUVARIABLE_H */