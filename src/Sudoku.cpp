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

void Solver::setup() {
    std::cout << "setup" << std::endl;
    // setup grid, gridValues, values
    std::vector<string> grid = { "4", "0", "0", "0", "0", "0", "8", "0", "5", "0", "3", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "7", "0", "0", "0", "0", "0", "0", "2", "0", "0", "0", "0", "0", "6", "0", "0", "0", "0", "0", "8", "0", "4", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "6", "0", "3", "0", "7", "0", "5", "0", "0", "2", "0", "0", "0", "0", "0", "1", "0", "4", "0", "0", "0", "0", "0", "0" };
    
    // To start, every square can be any digit; then assign values from the grid.
    for (int i = 0; i < data_.squares.size(); i++) {
        data_.cells[data_.squares[i]].value = grid[i];
    }

    for (auto& sq : data_.squares) {
        data_.cells[sq].printPeers();
        data_.cells[sq].printPossibles();
        data_.cells[sq].printValue();
        data_.cells[sq].printUnits();
        std::cout << "===" << std::endl;
    }


    // // this is just a test, take it out later
    // Cell cell = cells["A1"];
    // std::cout << "key: " << cell.key << std::endl;
    // std::cout << "value: " << cell.value << std::endl;
    // std::cout << "possibles: " << std::endl;
    // for (int i=0; i < cell.possibles.size(); i++) {
    //     std::cout << "\t" << cell.possibles[i] << std::endl;
    // }
    // std::cout << "possibles after: " << std::endl;
    // for (int i=0; i<cell.possibles.size(); i++) {
    //     std::cout << "\t" << cell.possibles[i] << std::endl;
    // }

}

void Solver::assign(Cell& cell, string& digit) {
}

void Solver::eliminate(Cell& cell, string& digit) {
}


} // namespace Sudoku