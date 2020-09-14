#ifndef SUDOKUMETAVARIABLE_H
#define SUDOKUMETAVARIABLE_H

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>
#include <set>

namespace Sudoku {

struct Cell;

struct Variables {
public:
    std::vector<int> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<std::string> rows = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
    std::vector<std::string> cols = GetStringsFromInts(digits);
    std::vector<std::string> squares = Cross(rows, cols);
    

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