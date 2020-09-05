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
    void getMinValuesKeys(std::unordered_map<std::string, int>& values, std::vector<std::string>& res);
    void insertValueToCells(std::vector<int>& grid, std::unordered_map<std::string, Cell>& cells);
    void eliminate(Cell& cell, int& digit);
    void assign(Cell& cell, int& digit);
    void search(std::unordered_map<std::string, Cell>& cells);
};


#endif /* SUDOKUSOLVER_H */
