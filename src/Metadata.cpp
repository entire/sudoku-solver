#include <sudoku/Metadata.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>

namespace Sudoku
{

Metadata::Metadata() {
    // setup unitlist
    std::vector<std::vector<std::string>> unitlist{};
    setupUnitlist(unitlist);

    // setup units
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> units;
    setupUnits(units, unitlist);

    // setup peers
    setupPeers(peers, units);
}

std::vector<std::string> Metadata::cross(std::vector<std::string>& A, std::vector<std::string>& B) {
    std::vector<std::string> cross;
    for (auto & i : A) {
        for (auto & j : B) {
            cross.push_back(i + j);
        }
    }
    return cross;
}

void Metadata::setupUnitlist(std::vector<std::vector<std::string>> &unitlist) {
    std::vector<std::vector<std::string>> colCross{};
    for (auto &c : cols) {
        std::vector<std::string> col{c};
        colCross.push_back(cross(rows, col));
    }

    std::vector<std::vector<std::string>> rowCross{};
    for (auto &r : rows) {
        std::vector<std::string> row{r};
        rowCross.push_back(cross(row, cols));
    }

    std::vector<std::vector<std::string>> rowUnitIdentifiers;
    std::vector<std::vector<std::string>> colUnitIdentifiers;
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

void Metadata::setupUnits(
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> &units,
    std::vector<std::vector<std::string>> &unitlist) {
    for (auto &s : squares) {
        for (auto &u : unitlist) {
            // if s exists in u
            if (std::find(u.begin(), u.end(), s) != u.end()) {
                // if s isn't in units hashmap, initialize it
                if (units.find(s) == units.end()) {
                    units[s] = std::vector<std::vector<std::string>>{};
                }
                // push_back u to units[s]
                std::vector<std::vector<std::string>> unit = units[s];
                unit.push_back(u);
                units[s] = unit;
            }
        }
    }
}

void Metadata::setupPeers(std::unordered_map<std::string, std::set<std::string>>& peers, std::unordered_map<std::string, std::vector<std::vector<std::string>>>& units) {
    for (auto &s : squares) {
        std::set<std::string> unitSet;
        for (auto &u : units[s]) {
            for (auto &sq : u) {
                if (sq != s) {
                    unitSet.insert(sq);
                }
            }
        }
        peers[s] = unitSet;
    }
}

} // namespace Sudoku