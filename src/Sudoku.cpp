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
    std::cout << std::endl;

    for (auto& sq : data_.squares) {
        // data_.cells[sq].printPeers();
        data_.cells[sq].printCandidtes();
        data_.cells[sq].printValue();
    //     data_.cells[sq].printUnits();
        // std::cout << "===" << std::endl;
    }
 
}

void Solver::insertValueToCells(std::vector<std::string>& grid, std::unordered_map<std::string, Cell>& cells) {
    // To start, every square can be any digit; then assign values from the grid.
    for (int i = 0; i < data_.squares.size(); i++) {
        if (grid[i] != "0") {
            assign(data_.cells[data_.squares[i]], grid[i]);
        }
    }
}

void Solver::assign(Cell& cell, string& digit) {
    // std::cout << "assigning : (" << digit << ") to cell: [" << cell.key << "]" << std::endl;
    
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
        // std::cout << "\t\tdoes not have candidate to eliminate for digit: " << digit << std::endl;
        return;
    }
    cell.removeCandidate(digit);
    // if cell candidate is greater than 1, remove from candidates 
    if (cell.candidates.size() == 0) {
        // std::cout << "\tcell value was 0 or null, key: " << cell.key << std::endl;
        return;
    } else if (cell.candidates.size() == 1) {
        // std::cout << "now removing from peers of " << cell.key << std::endl;
        for (auto& peer : cell.peers) {
            if (digit != "0") {
                eliminate(data_.cells[peer], cell.candidates[0]);
            }
        }
    }
    // std::cout << "\tremoved : " << digit << " from cell: " << cell.key << std::endl;
    for (auto& unit : cell.units) {
        std::vector<std::string> dplaces;
        for (auto& v : unit) {
        // std::cout << " is " << digit << " in " << data_.cells[v].key <<" candidates?"<<std::endl;
            if (data_.cells[v].hasCandidate(digit)) {
                dplaces.push_back(v);
            }
        }
        // std::cout << "\t\t\t key: " << cell.key << " digit: " << digit << " " << " dplaces: [ ";
        // for (auto& dplace : dplaces) {
            // std::cout << '"' << dplace << '"' << " ";
        // }
        // std::cout << "]" << std::endl;
        if (dplaces.size() == 0) {
            return;
        }
        if (dplaces.size() == 1) {
            // std::cout << "\tcalling assing from eliminate" << std::endl;
            assign(data_.cells[dplaces[0]], digit);
        }
    }
    // std::cout << "\t\t\t\t-- the end of 1 loop --" << std::endl;
}
void Solver::allCellsAreEqual(std::unordered_map<std::string, Cell>&cells) {
    for (auto& c : cells) {
        if (c.second.candidates.size() > 1) {
            return false;
        }
    }
    return true;
}

void Solver::search(Cell& cell) {
    // use DFS and propagation and try some values
    if (cell.candidates.size() == 0) {
        // nothing
    }

    if (allCellsAreEqual(data_.cells)) {
        std::cout << "solved" << std::endl;
    }
} // namespace Sudoku