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
    // use compiler generated default constructor
    Solver() = default;
    // don't generate a copy constructor
    Solver(const Solver&) = delete;
    // solve the sudoku
    void Solve();
    // print matrix for convenience
    static void PrintMatrix(std::vector<std::vector<int>>& matrix);
    static void GetAllValuesExcept(std::vector<int>& others, int& to_remove);
    static void PrintEvent(const std::string& message, const int& level, const int& digit, Cell& cell);

    bool InsertValueToCells(std::vector<int>& grid, std::unordered_map<std::string, Cell>& cells);
    bool Eliminate(Cell& cell, int& digit);
    bool Assign(Cell& cell, int& digit);
    bool Search(std::unordered_map<std::string, Cell>& cells, bool couldAssign, std::string caller);

    // check if series of cells solves the puzzle or not
    static bool isSolved(std::unordered_map<std::string , Cell>& cells);
};


#endif /* SUDOKUSOLVER_H */
