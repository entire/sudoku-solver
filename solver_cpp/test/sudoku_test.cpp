// tests.cpp
#include "../src/sudoku.cpp"
#include "../src/variables.cpp"
#include <gtest/gtest.h>

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