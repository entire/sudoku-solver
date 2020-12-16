#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>
#include "solver/variables.h"

namespace Sudoku
{

Variables::Variables() {
    std::vector<std::vector<std::string>> unitlist;
    // setup unitlist
    SetupUnitlist(unitlist);
    // setup units
    SetupCells(cells, unitlist);
    // setup peers
    SetupPeers(cells);
}

std::vector<std::string> Variables::Cross(std::vector<std::string>& A, std::vector<std::string>& B) {
    std::vector<std::string> cross;
    for (auto & i : A) {
        for (auto & j : B) {
            cross.push_back(i + j);
        }
    }
    return cross;
}

std::vector<std::string> Variables::GetStringsFromInts(std::vector<int>& ints) {
    std::vector<std::string> strings;
    for (auto& i : ints) {
        strings.push_back(std::to_string(i));
    }
    return strings;
}

void Variables::SetupUnitlist(std::vector<std::vector<std::string>> &unitlist) {
    std::vector<std::vector<std::string>> colCross{};
    for (auto &c : cols) {
        std::vector<std::string> col{c};
        colCross.push_back(Cross(rows, col));
    }

    std::vector<std::vector<std::string>> rowCross{};
    for (auto &r : rows) {
        std::vector<std::string> row{r};
        rowCross.push_back(Cross(row, cols));
    }

    std::vector <std::vector<std::string>> rowUnitIdentifiers;
    std::vector <std::vector<std::string>> colUnitIdentifiers;
    int unitSize = 3;           // sudoku is a 3 x 3 x 3 grid
    int lineSize = rows.size(); // line is row or column in a grid, 3 x 3
    for (int i = unitSize; i <= lineSize; i += unitSize) {
        int offset = i - lineSize;
        int start = i - unitSize;
        // split std::vectors into 3 elements each
        std::vector<std::string> rowUnit{rows.begin() + start, rows.end() + offset};
        std::vector<std::string> colUnit{cols.begin() + start, cols.end() + offset};
        rowUnitIdentifiers.push_back(rowUnit);
        colUnitIdentifiers.push_back(colUnit);
    }

    // get a std::vector of units
    std::vector<std::vector<std::string>> unitCross{};
    for (int k = 0; k < unitSize; k++) {
        for (int j = 0; j < unitSize; j++) {
            std::vector<std::string> unit{};
            for (auto &r : rowUnitIdentifiers[k]) {
                for (auto &c : colUnitIdentifiers[j]) {
                    unit.push_back(r + c);
                }
            }
            unitCross.push_back(unit);
        }
    }

    // insert colCross, rowCross, unitCross into unitlist
    unitlist.insert(unitlist.end(), colCross.begin(), colCross.end());
    unitlist.insert(unitlist.end(), rowCross.begin(), rowCross.end());
    unitlist.insert(unitlist.end(), unitCross.begin(), unitCross.end());
}

void Variables::SetupCells(std::unordered_map<std::string, Cell>& cells, std::vector<std::vector<std::string>>& unitlist) {
    for (auto &k : squares) {
        for (auto &u : unitlist) {
            // if k exists in u
            if (std::find(u.begin(), u.end(), k) != u.end()) {
                // if k isn't in cells hashmap, initialize it
                if (cells.find(k) == cells.end()) {
                    Cell cell;
                    cell.candidates = digits;
                    cell.key = k; 
                    cells[k] = cell;
                }
                // push_back u to units[s]
                cells[k].units.push_back(u);
            }
        }
    }
    // Cell cell = cells["C2"];
    // cell.PrintUnits();
}

void Variables::SetupPeers(std::unordered_map<std::string, Cell>& cells) {
    for (auto& k : squares) {
        std::set<std::string> peers;
        for (auto& unit : cells[k].units) {
            for (auto& v : unit) {
                if (v != k) {
                    peers.insert(v);
                }
            }
        }
        cells[k].peers = peers;
    }
    // Cell cell = cells["C2"];
    // cell.PrintPeers();
}

} // namespace Sudoku