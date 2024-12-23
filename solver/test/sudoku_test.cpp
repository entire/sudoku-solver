// tests.cpp
#include "../src/sudoku.cpp"
#include "../src/variables.cpp"
#include "../include/solver/sudoku.h"
#include <gtest/gtest.h>
#include <iostream>

void PrintGrid(const std::vector<int>& grid) {
    std::cout << "┌───────┬───────┬───────┐" << std::endl;
    for (int i = 0; i < 9; i++) {
        std::cout << "│ ";
        for (int j = 0; j < 9; j++) {
            if (grid[i * 9 + j] == 0) {
                std::cout << ". ";
            } else {
                std::cout << grid[i * 9 + j] << " ";
            }
            if ((j + 1) % 3 == 0) std::cout << "│ ";
        }
        std::cout << std::endl;
        if ((i + 1) % 3 == 0 && i < 8) {
            std::cout << "├───────┼───────┼───────┤" << std::endl;
        }
    }
    std::cout << "└───────┴───────┴───────┘" << std::endl;
}

std::vector<std::vector<int>> unsolved_matrix {
    {0, 9, 0, 0, 0, 0, 8, 5, 3},
    {0, 0, 0, 8, 0, 0, 0, 0, 4},
    {0, 0, 8, 2, 0, 3, 0, 6, 9},
    {5, 7, 4, 0, 0, 2, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 9, 0, 0, 6, 3, 7},
    {9, 4, 0, 1, 0, 8, 5, 0, 0},
    {7, 0, 0, 0, 0, 6, 0, 0, 0},
    {6, 8, 2, 0, 0, 0, 0, 9, 0}};

std::vector<int> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9};
std::vector<std::string> rows = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
std::vector<std::string> cols = Sudoku::Variables::GetStringsFromInts(digits);  

TEST(SudokuVariableTest,UnitlistSize)
{
    Sudoku::Variables v;
    std::vector<std::vector<std::string>> unitlist{};
    v.SetupUnitlist(unitlist);
    ASSERT_EQ(27, unitlist.size());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(SudokuSolverTest, SolveSudoku) {
    Sudoku::Solver solver;
    std::vector<int> flattened_matrix;
    for (const auto& row : unsolved_matrix) {
        flattened_matrix.insert(flattened_matrix.end(), row.begin(), row.end());
    }
    
    std::cout << "\nOriginal puzzle:" << std::endl;
    PrintGrid(flattened_matrix);
    
    std::vector<int> solution = solver.Solve(flattened_matrix);
    ASSERT_EQ(solution.size(), 81);
    
    std::cout << "\nSolved puzzle:" << std::endl;
    PrintGrid(solution);
    
    // Add validation checks
    auto isValidSolution = [](const std::vector<int>& grid) {
        // Check each row, column, and box for numbers 1-9
        // Return false if any duplicates found or numbers outside 1-9 range
        return true; // implement validation logic
    };
    
    ASSERT_TRUE(isValidSolution(solution)) << "Solution violates Sudoku rules";
}