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

enum class GameState {
  uninitialized,
  inprogress,
  failed,
  complete
};

struct Game {
public:
    Game() = default;
    Game(std::shared_ptr<Game>& g) : cells(g->cells) {}
    Game(const std::unordered_map<std::string, Cell> cells) : cells(cells) {}

    // this conatins all the cells for the game that it's currently running
    std::unordered_map<std::string, Cell> cells;
    // current game state
    GameState state = GameState::inprogress;
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

    void InsertValueToCells(std::vector<int>& grid, std::shared_ptr<Game>& game);
    bool Assign(std::shared_ptr<Game>& game, std::string& key, int& digit);
    bool Eliminate(std::shared_ptr<Game>& game, std::string& key, int& digit);
    std::shared_ptr<Game> Search(std::shared_ptr<Game>& game);

    // check if series of cells solves the puzzle or not
    static bool isSolved(std::unordered_map<std::string , Cell>& cells);
};


}
#endif /* SUDOKUSOLVER_H */
