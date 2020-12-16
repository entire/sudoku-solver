#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <memory>
#include "solver/sudoku.h"
#include "solver/variables.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace Sudoku
{

void Solver::Solve(const std::vector<int>& grid) {

    auto game = std::make_shared<Game>(data_.cells);
    // init game with cells
    bool success = SetupGameWithGrid(grid, game);
    if (!success) {
        std::cout << "could not setup game! please check your grid." << std::endl;
    }
     
    // search all cells, returns solved solution
    std::shared_ptr<Game> finished = Search(game);

    for (auto& sq : data_.squares) {
        // finished->cells[sq].PrintPeers();
        // finished->cells[sq].PrintCandidtes();
        // finished->cells[sq].PrintValue();
        // finished->cells[sq].PrintUnits();
        // std::cout << === << std::endl;
    }
}

bool Solver::SetupGameWithGrid(const std::vector<int>& grid, std::shared_ptr<Game>& game) {
    // To start, every square can be any digit; then assign values from the grid.
    bool success = true;
    for (int i = 0; i < data_.squares.size(); i++) {
        if (grid[i] != 0) {
            if (!Assign(game, data_.squares[i], grid[i])) {
                success = false;
            }
        }
    }
    return success;
}

bool Solver::Assign(std::shared_ptr<Sudoku::Game>& game, std::string key, int digit) {
    Cell& cell = game->cells[key];
    PrintEvent("assigning", 1, digit, cell);
    
    // get all values besides possible candidates that work with that cell
    GetAllValuesExcept(cell.candidates, digit);

    // Eliminate all other values that aren't the assign value from candidate
    for (auto& candidate : cell.candidates) {
        // we're asking if this 
        if (!Eliminate(game, key, candidate)) {
            return false;
        }
    } 
    return true;
}

void Solver::GetAllValuesExcept(std::vector<int>& others, int to_remove) {
    others.erase(
        std::remove_if(others.begin(), others.end(),
        [&](int& s) { return s == to_remove; }),    
    others.end());
}

void Solver::PrintEvent(const std::string& message, const int level, const int digit, Cell& cell) {
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

bool Solver::Eliminate(std::shared_ptr<Game>& game, std::string key, int digit) {
    // if digit doesn't exist in cell value candidates return 
    Cell& cell = game->cells[key];
    if (!cell.HasCandidate(digit)) {
        return true;
    }
    cell.RemoveCandidate(digit);
    PrintEvent("eliminated", 1, digit, cell);

    // if cell candidate is greater than 1, remove from candidates 
    if (cell.candidates.empty()) {
        PrintEvent("contradiction:eliminate", 0, digit, cell);
        return false;
    } else if (cell.candidates.size() == 1) {
        PrintEvent("eliminate.peers", 2, digit, cell);
        for (auto peer : cell.peers) {
            if (digit != 0 && cell.candidates.size() > 0) {
                // std::cout << "eliminate d:" << digit << " from cell: " << data_.cells[peer].key << " which is a peer of " << cell.key << std::endl;
                if (!Eliminate(game, peer, cell.candidates[0])) {
                    std::cout << "eliminate 2 returned false!" << std::endl;
                    return false;
                }
            }
        }
    }
    // by using process of elimination, for squares that have one possible value
    // see if we can see if we can assign a value there
    for (auto& unit : cell.units) {
        std::vector<std::string> dplaces;
        for (auto& v : unit) {
            if (game->cells[v].HasCandidate(digit)) {
                dplaces.push_back(v);
            }
        }
        if (dplaces.empty()) {
            return false;
        }
        if (dplaces.size() == 1) {
            if (!Assign(game, dplaces[0], digit)) {
                return false;
            }
        }
    }
    // if it did not fail and return, it must have worked
    return true;
}

std::shared_ptr<Game> Solver::Search(std::shared_ptr<Game>& game) {
    // check if puzzle is solved
    if (isSolved(game->cells)) {
        game->state = GameState::complete;
        std::cout << "solved!" << std::endl;
        return std::move(game);
    }

    // check if GameState is failed
    if (game->state == GameState::failed) { 
        std::cout << "search returned false! 1" << std::endl;
        return std::move(game);
    }

    // get unordered_map with key of sq and value of cell candidate size
    std::unordered_map<std::string, int> pos;
    for (auto& s : data_.squares) {
        Cell cell = game->cells[s];
        if (cell.candidates.size() > 1) {
           pos[cell.key] = cell.candidates.size();
        }
    }
    
    // get keys for possible undetermined s values
    std::vector<std::string> res; 
    res.reserve(pos.size());
    for (auto& p : pos) {
        res.push_back(p.first);
    }
   
    // then sort res vector
    std::sort(res.begin(), res.end(), 
        [&](std::string a, std::string b) {
            if (game->cells[a].candidates.size() == game->cells[b].candidates.size()) {
                return a < b;
            } else {
                return game->cells[a].candidates.size() < game->cells[b].candidates.size();
            }
        });

    // then get the key of square w/ smallest candidate.size()
    std::string min_cand_key = res[0];

    // search for a non-failing solution and keep the results to see if they failed
    std::vector<std::shared_ptr<Game>> search_results;
    for (auto& p : game->cells[min_cand_key].candidates) {
        // make a copy of shared_ptr to see if this possible value will work out
        // without polluting our base `game` ptr
        auto game_to_test = std::make_shared<Game>(game);
        // check if it fails to assign / eliminate properly
        if (!Assign(game_to_test, min_cand_key, p)) {
            game_to_test->state = GameState::failed;
        }
        search_results.push_back(std::move(Search(game_to_test)));
    }

    // iterate through search results (i.e. possible games)
    for (auto search_result : search_results) {
        if (search_result->state == GameState::failed) { // search
            std::cout << "it failed" << std::endl;
        } else {
            std::cout << "it worked" << std::endl;
            // run it one more time to check to see if the puzzle is solved yet
            return std::move(Search(search_result));
        }
    }
    // since there are more possible wrong configurations, wait to see if iterating through
    // search results produced any good results, if not just returned the first one in the
    // search_results which will have a GameStats::failed
    return std::move(search_results[0]);
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

Solver::~Solver() {}

} // namespace Sudoku



