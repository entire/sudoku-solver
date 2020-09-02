#include <sudoku/Sudoku.h>
#include <sudoku/Metadata.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>

#define UNASSIGNED 0

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace Sudoku
{

void Solver::printMatrix(vector<vector<int>> &matrix)
{
    cout << endl;
    for (int row = 0; row < matrix.size(); row++) {
        // add a line every third row
        if (row % 3 == 0 && row != 0) {
            cout << "------------+-------------+------------";
            cout << endl;
        }
        // now go through columns
        for (int col = 0; col < matrix.size(); col++) {
            // add a line every third col
            if (col % 3 == 0 && col != 0) {
                cout << "|  ";
            } else {
                cout << " ";
            }
            cout << matrix[row][col] << "  ";
        }
        // line break
        cout << endl;
    }
}


void Solver::solve() {
    std::vector<std::string> grid = { "4", "0", "0", "0", "0", "0", "8", "0", "5", "0", "3", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "7", "0", "0", "0", "0", "0", "0", "2", "0", "0", "0", "0", "0", "6", "0", "0", "0", "0", "0", "8", "0", "4", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "6", "0", "3", "0", "7", "0", "5", "0", "0", "2", "0", "0", "0", "0", "0", "1", "0", "4", "0", "0", "0", "0", "0", "0" };

    // step 1. insert values to the cells
    insertValueToCells(grid, data_.cells);

    for (auto& sq : data_.squares) {
        // data_.cells[sq].printPeers();
        // data_.cells[sq].printCandidtes();
        // data_.cells[sq].printValue();
    //     data_.cells[sq].printUnits();
        // std::cout << "===" << std::endl;
    }
}

void Solver::insertValueToCells(std::vector<std::string>& grid, std::unordered_map<std::string, Cell>& cells) {
    // To start, every square can be any digit; then assign values from the grid.
    for (int i = 0; i < data_.squares.size(); i++) {
        assign(data_.cells[data_.squares[i]], grid[i]);
    }
}

void Solver::assign(Cell& cell, string& digit) {
    // check if cell is empty, if not assign
    if (cell.value.empty() || cell.value == "0") {
        cell.value = digit;
    }
    if (cell.value == "0") { // return early
        return;
    }
    // get all values besides candidate
    std::vector<std::string> other_values = cell.candidates;
    other_values.erase(
        std::remove_if(other_values.begin(), other_values.end(),
        [&](std::string& s) { return s == digit; }),
    other_values.end());
    // eliminate all other values that aren't the assigned value from candidate
    for (auto& other : other_values) {
        eliminate(cell, other);
    } 
}

void Solver::eliminate(Cell& cell, string& digit) {
    // if digit doesn't exist in cell value candidates return 
    if (!cell.hasCandidate(digit)) {
        return;
    }
    // if cell candidate is greater than 1, remove from candidates 
    if (cell.candidates.size() > 1) {
        cell.removeCandidate(digit);
    }
    // if cell candidates is 1, it means we have the answer, remove value from all peer candidates
    if (cell.candidates.size() == 1) {
        std::cout << "cell : [" << cell.key << "] only has : " << cell.candidates[0] << std::endl;
        for (auto& peer : cell.peers) {
            if (digit != "0") {
                eliminate(data_.cells[peer], cell.candidates[0]);
            }
        }
    }
    
}


} // namespace Sudoku