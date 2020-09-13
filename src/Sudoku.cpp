#include <sudoku/Sudoku.h>
#include <sudoku/Metadata.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace Sudoku
{

void Solver::PrintMatrix(vector<vector<int>> &matrix)
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


void Solver::Solve() {
    std::vector<int> grid = { 4, 0, 0, 0, 0, 0, 8, 0, 5, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 8, 0, 4, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 6, 0, 3, 0, 7, 0, 5, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 0 };

    // step 1. insert values to the cells
    bool couldAssign = InsertValueToCells(grid, data_.cells);
     
    // search all cells
    Search(data_.cells, true, "Init");

    for (auto& sq : data_.squares) {
        // data_.cells[sq].PrintPeers();
        data_.cells[sq].PrintCandidtes();
        // data_.cells[sq].PrintValue();
    //     data_.cells[sq].PrintUnits();
        // std::cout << === << std::endl;
    }

}

bool Solver::InsertValueToCells(std::vector<int>& grid, std::unordered_map<std::string, Cell>& cells) {
    // To start, every square can be any digit; then assign values from the grid.
    bool couldAssign = true;
    for (int i = 0; i < data_.squares.size(); i++) {
        if (grid[i] != 0) {
            if (!Assign(data_.cells[data_.squares[i]], grid[i])) {
                couldAssign = false;
            }
        }
    }
    return false;
}

bool Solver::Assign(Cell& cell, int& digit) {
    PrintEvent("assigning", 1, digit, cell);
    
    // get all values besides candidate
    std::vector<int> other_values = cell.candidates;
    GetAllValuesExcept(other_values, digit);

    // Eliminate all other values that aren't the assign value from candidate
    for (auto& other : other_values) {
        if (!Eliminate(cell, other)) {
            std::cout << "assign false!" << std::endl;
            return false;
        }
    } 
    return true;
}

 void Solver::GetAllValuesExcept(std::vector<int>& others, int& to_remove) {
    others.erase(
        std::remove_if(others.begin(), others.end(),
        [&](int& s) { return s == to_remove; }),    
    others.end());
}

void Solver::PrintEvent(const std::string& message, const int& level, const int& digit, Cell& cell) {
    std::string tabLevel(level, '\t');
    std::cout << tabLevel << message << ": ";
    std::cout << "cell: " << cell.key << " ";
    std::cout << "digit: " << digit << " ";
    std::cout << "values[s]: ";
    for (auto& c : cell.candidates) {
        std::cout << c;
    }
    std::cout << std::endl;
}

bool Solver::Eliminate(Cell& cell, int& digit) {
    // if digit doesn't exist in cell value candidates return 
    if (!cell.HasCandidate(digit)) {
        // std::cout << "\t\t" << cell.key << " does not have candidate to Eliminate for digit: " << digit << std::endl;
        return true;
    }
    PrintEvent("eliminate", 1, digit, cell);
    cell.RemoveCandidate(digit);

    // if cell candidate is greater than 1, remove from candidates 
    if (cell.candidates.empty()) {
        PrintEvent("contradiction:eliminate", 0, digit, cell);
        return false;
    } else if (cell.candidates.size() == 1) {
        PrintEvent("eliminate.peers", 2, digit, cell);
        for (auto& peer : cell.peers) {
            if (digit != 0 && cell.candidates.size() > 0) {
                // std::cout << "eliminate d:" << digit << " from cell: " << data_.cells[peer].key << " which is a peer of " << cell.key << std::endl;
                if (!Eliminate(data_.cells[peer], cell.candidates[0])) {
                    std::cout << "eliminate 2 returned false!" << std::endl;
                    return false;
                }
            }
        }
    }
    for (auto& unit : cell.units) {
        std::vector<std::string> dplaces;
        for (auto& v : unit) {
        // std::cout << " is " << digit << " in " << data_.cells[v].key <<" candidates?"<<std::endl;
            if (data_.cells[v].HasCandidate(digit)) {
                dplaces.push_back(v);
            }
        }
        // std::cout << "\t\t\t key: " << cell.key << " digit: " << digit << " " << " dplaces: [ ";
        // for (auto& dplace : dplaces) {
        //     std::cout << '"' << dplace << '"' << " ";
        // }
        // std::cout << "]" << std::endl;
        if (dplaces.empty()) {
            // std::cout << "\tdplaces is zero" << std::endl;
            std::cout << "Eliminate 3 returned false!" << std::endl;
            return false;
        }
        if (dplaces.size() == 1) {
            // std::cout << "\tcalling assing from Eliminate" << std::endl;
            if (!Assign(data_.cells[dplaces[0]], digit)) {
                std::cout << "Eliminate 4 returned false!" << std::endl;
                return false;
            }
        }
    }
    // if it did not fail, return true
    return true;
}

bool Solver::Search(std::unordered_map<std::string, Cell>& cells, bool couldAssign, std::string caller) {
    std::cout << std::endl; 
    std::cout << std::endl; 
    std::cout << std::endl; 
    std::cout << std::endl; 
    std::cout << std::endl; 
    std::cout << "ok search" << std::endl;
    std::cout << "called by: " << caller << std::endl;

    // if could Assign was false
    if (!couldAssign) { 
        std::cout << "search returned false! 1" << std::endl;
        return false;
    }

    // check if puzzle is solved
    if (isSolved(cells)) {
        return true;
    }

    // get unordered_map with key of sq and value of cell candidate size
    std::unordered_map<std::string, int> pos;
    for (auto& s : data_.squares) {
        Cell cell_ = cells[s];
        if (cell_.candidates.size() > 1) {
           pos[cell_.key] = cell_.candidates.size();
        }
    }
    
    // get keys for possible undetermined s values
    std::vector<std::string> res; 
    res.reserve(pos.size());
    for (auto& p : pos) {
        res.push_back(p.first);
    }
   
    // sort res vector
    std::sort(res.begin(), res.end(), 
        [&](std::string a, std::string b) {
            if (cells[a].candidates.size() == cells[b].candidates.size()) {
                return a < b;
            } else {
                return cells[a].candidates.size() < cells[b].candidates.size();
            }
        });

    // for (auto& r : res) {
    //     std::cout << r << " : " << cells[r].candidates.size() << std::endl;
    //     std::cout << "candidates: ";
    //         for (auto& v : cells[r].candidates) {
    //             std::cout << v;
    //         }
    //         std::cout << std::endl;
    // }
 
    // find new cells to Assign and search
    bool search_success = true;
    for (std::string& s : res) {
        std::unordered_map<std::string, Cell> to_search;
        for (auto& p : cells[s].candidates) {
            std::cout << "size n:" << cells[s].candidates.size() << " and s:" << s << " for values[s]: ";
            for (auto& v : cells[s].candidates) {
                std::cout << v;
            }
            std::cout << std::endl;
            std::cout << "before search" << std::endl;
            
            if (!Search(cells, Assign(cells[s], p), s)) {
                std::cout << "after search" << std::endl;
                std::cout << "!" << std::endl;
            }
        }
    }
    return search_success;
    
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



