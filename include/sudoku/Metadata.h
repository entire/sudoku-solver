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
    std::string value;
    // vector of units
    std::vector<std::vector<std::string>> units;
    // all possible cell values for this cell
    std::vector<std::string> possibles;
    // peers
    std::set<std::string> peers;
 
    // remove by target
    void remove(std::string& target) {
        possibles.erase(
            std::remove_if(possibles.begin(), possibles.end(),
            [&](std::string& s) { return s == target; }),
        possibles.end());
   }

    // check if it has string
    bool has(std::string& s) {
        return std::find(possibles.begin(), possibles.end(), s) != possibles.end();
    }
    // print the value of the cell
    void printValue() {
        if (!value.empty()) {
            std::cout << "value at " << key << " is " << value << std::endl;
        } else {
            std::cout << "value is empty or has not been initialized.";
        }
    }
    // print all possible values
    void printPossibles() {
        std::cout << "possibles for: " << key << std::endl;
        std::cout << "[ ";
        for (auto& p : possibles) {
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
    std::vector<std::string> digits = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
    std::vector<std::string> rows = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
    std::vector<std::string> cols = digits;
    std::vector<std::string> squares = cross(rows, cols);
    std::unordered_map<std::string, Cell> cells;
    std::vector<std::string> cross(std::vector<std::string>& A, std::vector<std::string>& B);
 
    // default constructor
    Metadata();
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