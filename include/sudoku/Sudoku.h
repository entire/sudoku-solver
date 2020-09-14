#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>
#include <sudoku/Variables.h>
namespace Sudoku {
struct Cell {
public:
   // this is the key of the cell, e.g. "A2", "C3", etc
    std::string key;
    // this is the value of the cell
    int value;
    // vector of units
    std::vector<std::vector<std::string>> units;
    // all possible cell values for this cell
    std::vector<int> candidates;
    // peers
    std::set<std::string> peers;
 
    // remove by target
    void RemoveCandidate(int& target) {
        // if (candidates.size() == 1) {
        //     return;
        // }
        // std::cout << "key: " << this->key << " - RemoveCandidate: " << target << " - cand size: " << candidates.size() << std::endl;
        candidates.erase(
            std::remove_if(candidates.begin(), candidates.end(),
            [&](int& s) { return s == target; }),
        candidates.end());
        // std::cout << "post remove canditate size: " << candidates.size() << std::endl;
   }

    // check if the candidate has number
    bool HasCandidate(int& s) {
        return std::find(candidates.begin(), candidates.end(), s) != candidates.end();
    }

    // print the value of the cell
    void PrintValue() {
        if (value == 0) {
            std::cout << "value at " << key << " is " << value << std::endl;
        } else {
            std::cout << "value is empty or has not been initialized." << std::endl;
        }
    }
    // print all possible candidates
    void PrintCandidtes() {
        std::cout << "candidate for: " << key << std::endl;
        std::cout << "[ ";
        for (auto& p : candidates) {
            std::cout << p << " ";
        }
        std::cout << "]" << std::endl;
    }
    // print all units associated with cell
    void PrintUnits() {
        std::cout << "units for: " << key << std::endl;
        for (auto& unit : units) {
            std::cout << "[ ";
            for (auto& v : unit) {
                std::cout << v << " ";
            }
            std::cout << "]" << std::endl;
        }
    }
    // print all peers associated with cell
    void PrintPeers() {
        std::cout << "peers for: " << key << std::endl;
        std::cout << "[ ";
        for (auto& peer : peers) {
            std::cout << peer << " ";
        }
        std::cout << "]" << std::endl;
    }
};

struct Game {
public:
    Game() = default;
    Game(const Game& g) : cells(g.cells) {}
    Game(std::shared_ptr<Game>& g) : cells(g->cells) {}
    Game(const std::unordered_map<std::string, Cell> cells) : cells(cells) {}

    // this conatins all the cells for the game that it's currently running
    std::unordered_map<std::string, Cell> cells;

    void worksForDigit(int digit) {
       std::cout << "works for digit" << std::endl; 
    }
};
class Solver {
private:
    std::vector<std::vector<int>> matrix_;
 
    // default game variables
    Variables data_;

public:
    // use compiler generated default constructor
    Solver() = default;
    // don't generate a copy constructor
    Solver(const Solver&) = delete;
    // solve the sudoku
    void Solve();
    // print matrix for convenience
    static void GetAllValuesExcept(std::vector<int>& others, int& to_remove);
    static void PrintEvent(const std::string& message, const int& level, const int& digit, Cell& cell);

    bool InsertValueToCells(std::vector<int>& grid, std::shared_ptr<Game>& game);
    bool Assign(std::shared_ptr<Game>& game, std::string& key, int& digit);
    bool Eliminate(std::shared_ptr<Game>& game, std::string& key, int& digit);
    bool Search(std::shared_ptr<Game>& game, bool couldAssign, std::string caller);

    // check if series of cells solves the puzzle or not
    static bool isSolved(std::unordered_map<std::string , Cell>& cells);
};


}
#endif /* SUDOKUSOLVER_H */
