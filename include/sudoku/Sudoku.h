#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <iostream>
#include <algorithm>
#include <sudoku/Metadata.h>

// individual cell of grid
struct Cell {
    // contains all possible cell values
    // if possibles.size() == 1, we know value of the cell
    std::vector<std::string> possibles;
    // this is the key of the cell, e.g. "A2", "C3", etc
    std::string key;
    // this is the value of the cell
    std::string value;


    // remove by target
    void remove(std::string& target) {
        possibles.erase(
            std::remove_if(possibles.begin(), possibles.end(),
            [&](std::string& s) { return s == target; }),
        possibles.end());
        for (auto& p : possibles) {
            std::cout << "p: " << p << std::endl;
        }
    }

    // check if it has string
    bool has(std::string& s) {
        return std::find(possibles.begin(), possibles.end(), s) != possibles.end();
    } 
};

namespace Sudoku {
    class Solver;
}

class Sudoku::Solver {
private:
    std::vector<std::vector<int>> matrix_;
 
    // default metadata about sudoku
    Metadata data_;
    std::unordered_map<std::string, Cell> cells_;

public:
    // print matrix for convenience
    void printMatrix(std::vector<std::vector<int>>& matrix);
    void setup();
    void setupCells(std::vector<std::string>& grid, std::unordered_map<std::string, Cell> cells);

    void eliminate(Cell &cell, std::string &digit);
    void assign(Cell &cell, std::string &digit);
};


#endif /* SUDOKUSOLVER_H */
