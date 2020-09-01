#ifndef SUDOKUMETADATA_H
#define SUDOKUMETADATA_H

#include <vector>
#include <unordered_map>
#include <string>
#include <set>

namespace Sudoku {

struct Metadata {
public:
    std::vector<std::string> digits = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
    std::vector<std::string> rows = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
    std::vector<std::string> cols = digits;
    std::vector<std::string> squares = cross(rows, cols);
    std::unordered_map<std::string, std::set<std::string>> peers;

    // default constructor
    Metadata();

    // get cross product from A & B
    std::vector<std::string> cross(std::vector<std::string>& A, std::vector<std::string>& B);
    
    // setting up unitlist
    void setupUnitlist(std::vector<std::vector<std::string>>& unitlist);
    
    // setup units
    void setupUnits(
        std::unordered_map<std::string, std::vector<std::vector<std::string>>>& units,
        std::vector<std::vector<std::string>>& unitlist
    );

    // setup peers
    void setupPeers(
        std::unordered_map<std::string, std::set<std::string>>& peers,
        std::unordered_map<std::string, std::vector<std::vector<std::string>>>& units
    );

    ~Metadata() = default;
};

} // namespace Sudoku

#endif /* SUDOKUMETADATA_H */