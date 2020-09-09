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
    std::vector<int> grid = { 4, 0, 0, 0, 0, 0, 8, 0, 5, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 8, 0, 4, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 6, 0, 3, 0, 7, 0, 5, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 0 };

    // step 1. insert values to the cells
    insertValueToCells(grid, data_.cells);
     
    // search all cells
    search(data_.cells);
    std::cout << std::endl;

    for (auto& sq : data_.squares) {
        // data_.cells[sq].printPeers();
        data_.cells[sq].printCandidtes();
        // data_.cells[sq].printValue();
    //     data_.cells[sq].printUnits();
        // std::cout << === << std::endl;
    }

}

void Solver::insertValueToCells(std::vector<int>& grid, std::unordered_map<std::string, Cell>& cells) {
    // To start, every square can be any digit; then assign values from the grid.
    for (int i = 0; i < data_.squares.size(); i++) {
        if (grid[i] != 0) {
            assign(data_.cells[data_.squares[i]], grid[i]);
        }
    }
}

void Solver::assign(Cell& cell, int& digit) {
    std::cout << "assigning d:" << digit << " to cell: " << cell.key << " " << "for values: ";
    for (auto& c : cell.candidates) {
        std::cout << c;
    }
    std::cout << std::endl;
    
    // get all values besides candidate
    std::vector<int> other_values = cell.candidates;
    getAllValuesExcept(other_values, digit);

    // eliminate all other values that aren't the assigned value from candidate
    for (auto& other : other_values) {
        eliminate(cell, other);
    } 
}

 void Solver::getAllValuesExcept(std::vector<int>& others, int& to_remove) {
    others.erase(
        std::remove_if(others.begin(), others.end(),
        [&](int& s) { return s == to_remove; }),
    others.end());
}

void Solver::eliminate(Cell& cell, int& digit) {
    // if digit doesn't exist in cell value candidates return 
    // std::cout << "\t1. possible size: " << cell.candidates.size() << std::endl;
    if (!cell.hasCandidate(digit)) {
        // std::cout << "\t\tdoes not have candidate to eliminate for digit: " << digit << std::endl;
        return;
    }
    cell.removeCandidate(digit);
    std::cout << "\t1. possible size: " << cell.candidates.size() << std::endl;

    // if cell candidate is greater than 1, remove from candidates 
    if (cell.candidates.size() == 0) {
        // std::cout << "\tcell value was 0 or null, key: " << cell.key << std::endl;
        return;
    } else if (cell.candidates.size() == 1) {
        // std::cout << "there's only one, now removing from peers of " << cell.key << std::endl;
        for (auto& peer : cell.peers) {
            if (digit != 0 && cell.candidates.size() > 0) {
                // std::cout << "eliminate d:" << digit << " from peers of cell: " << cell.key << std::endl;
                eliminate(data_.cells[peer], cell.candidates[0]);
            }
        }
    }
    std::cout << "\tremoved : " << digit << " from cell: " << cell.key << std::endl;
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
            std::cout << "\tdplaces is zero" << std::endl;
            return;
        }
        if (dplaces.size() == 1) {
            std::cout << "\tcalling assing from eliminate" << std::endl;
            assign(data_.cells[dplaces[0]], digit);
        }
    }
    // std::cout << "\t\t\t\t-- the end of 1 loop --" << std::endl;
}

void Solver::getMinValuesKeys(std::unordered_map<std::string, int>& values, std::vector<std::string>& res) {
    // get minimum value
    auto it = *std::min_element(std::begin(values), std::end(values),
                        [](const auto& l, const auto& r) { 
                            return l.second < r.second; 
                        });
    int min_value = std::get<1>(it);

    // get keys for all the min values
    for (auto& v : values) {
        if (v.second == min_value) {
            res.push_back(v.first);
        }
    }
}

void Solver::search(std::unordered_map<std::string, Cell>& cells) {
    std::cout << std::endl; 
    std::cout << std::endl; 
    std::cout << std::endl; 
    std::cout << std::endl; 
    std::cout << std::endl; 
    std::cout << "ok search" << std::endl;

    // check if puzzle is solved
    if (isSolved(cells)) {
        return;
    }

    // get unordered_map with key of sq and value of cell candidate size
    std::unordered_map<std::string, int> values;
    for (auto& s : data_.squares) {
        Cell cell_ = data_.cells[s];
        if (cell_.candidates.size() > 1) {
           values[cell_.key] = cell_.candidates.size();
        }
    }

    // get keys for min values 
    std::vector<std::string> res; 
    getMinValuesKeys(values, res);

    // sort res vector
    std::sort(res.begin(), res.end(), 
        [](std::string a, std::string b) {
            return a < b;
        });
   
    // find new cells to assign and search
    for (auto& s : res) {
        std::unordered_map<std::string, Cell> to_search;
        for (auto& p : data_.cells[s].candidates) {
            std::cout << "size n:" << data_.cells[s].candidates.size() << " and s:" << s << " for values[s]: ";
            for (auto& v : data_.cells[s].candidates) {
                std::cout << v;
            }
            std::cout << std::endl;
            assign(data_.cells[s], p);
            search(cells);
        }
    }
}

bool Solver::isSolved(std::unordered_map<std::string , Cell>& cells) {
    bool solved = true;
    for (auto& c : cells) {
        if (c.second.candidates.size() > 1) {
            solved = false;
            break;
        }
    }
    return solved;


}

} // namespace Sudoku



