#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <iostream>
#include <algorithm>
#include <sudoku/Metadata.h>

namespace Sudoku {
    class Solver;
}

class Sudoku::Solver {
private:
    std::vector<std::vector<int>> matrix_;
 
    // default metadata about sudoku
    Metadata data_;

public:
    // print matrix for convenience
    void printMatrix(std::vector<std::vector<int>>& matrix);
    void solve();
    void insertValueToCells(std::vector<std::string>& grid, std::unordered_map<std::string, Cell>& cells);
    void eliminate(Cell& cell, std::string& digit);
    void assign(Cell& cell, std::string& digit);
};


#endif /* SUDOKUSOLVER_H */
