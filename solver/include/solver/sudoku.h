#ifndef SOLVER_SUDOKU_H
#define SOLVER_SUDOKU_H

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>
#include <solver/variables.h>

namespace Sudoku {

/** 
 * \enum GameState
 * This tracks the state of where the sudoku game is.
 */

enum class GameState {
  uninitialized,
  inprogress,
  failed,
  complete
};


/**
 *  \struct Game
 *  \brief This is the default game that is tracked and used to see if we can solve it or not
 */
struct Game {
public:
    Game() = default;
    Game(std::shared_ptr<Game>& g) : cells(g->cells) {}
    Game(const std::unordered_map<std::string, Cell> cells) : cells(cells) {}
    // \brief this contains all the cells for the game that it's currently running
    std::unordered_map<std::string, Cell> cells;
    // current game state
    GameState state = GameState::uninitialized;
};

/**
 *  \class Solver
 *  \brief This is the class that handles the constraint propagation and search algorithm
 */
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
    // entry function to solve the sudoku given grid
    void Solve(const std::vector<int>& grid);
    // get all values except to_remove value from unit
    static void GetAllValuesExcept(std::vector<int>& others, int to_remove);
    // convenience function for debugging
    static void PrintEvent(const std::string& message, const int level, const int digit, Cell& cell);
    // setup function to insert cells and setup a Game 
    bool SetupGameWithGrid(const std::vector<int>& grid, std::shared_ptr<Game>& game);
    // assign a value to a grid at square
    bool Assign(std::shared_ptr<Game>& game, std::string key, int digit);
    // eliminate a value from a grid at square 
    bool Eliminate(std::shared_ptr<Game>& game, std::string key, int digit);
    // recursive search function to try out different possible game configurations
    std::shared_ptr<Game> Search(std::shared_ptr<Game>& game);
    // check if series of cells solves the puzzle or not
    static bool isSolved(std::unordered_map<std::string , Cell>& cells);

    // destructor
    ~Solver();
};


}
#endif /* SOLVER_SUDOKU_H */
