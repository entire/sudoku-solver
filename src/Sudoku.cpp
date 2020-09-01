#include <sudoku/Sudoku.h>
#include <sudoku/Metadata.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>

#define UNASSIGNED 0

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace Sudoku
{

void Solver::printMatrix(vector<vector<int>> &matrix)
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

void Solver::setup() {
    // setup grid, gridValues, values
    std::vector<string> grid = { "4", "0", "0", "0", "0", "0", "8", "0", "5", "0", "3", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "7", "0", "0", "0", "0", "0", "0", "2", "0", "0", "0", "0", "0", "6", "0", "0", "0", "0", "0", "8", "0", "4", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "6", "0", "3", "0", "7", "0", "5", "0", "0", "2", "0", "0", "0", "0", "0", "1", "0", "4", "0", "0", "0", "0", "0", "0" };
    setupCells(grid, cells_);
}

void Solver::setupCells(std::vector<string>& grid, std::unordered_map<string, Cell> cells) {

    // To start, every square can be any digit; then assign values from the grid.
    for (int i = 0; i < grid.size(); i++) {
        string key = data_.squares[i];
        // and set up cells
        Cell cell;
        cell.possibles = data_.digits;
        cell.key = key;
        cell.value = grid[i];
        cells[key] = cell;
    }

    // this is just a test, take it out later
    Cell cell = cells["A1"];
    std::cout << "key: " << cell.key << std::endl;
    std::cout << "value: " << cell.value << std::endl;
    std::cout << "possibles: " << std::endl;
    for (int i=0; i < cell.possibles.size(); i++) {
        std::cout << "\t" << cell.possibles[i] << std::endl;
    }
    std::cout << "possibles after: " << std::endl;
    for (int i=0; i<cell.possibles.size(); i++) {
        std::cout << "\t" << cell.possibles[i] << std::endl;
    }

    assign(cell, cell.value);

}

void Solver::assign(Cell& cell, string& digit) {
    std::cout << "cell value: " << cell.value << std::endl;
    std::cout << "assign" << std::endl;

    // remove it self if cell contains digit
	if (cell.has(digit)){
        std::cout << "removing digit" << std::endl;
    	cell.remove(digit);
    }

	//note this is the correct implementation of the "all" function due to its short-circuiting feature
	for (int i =  0; i < cell.possibles.size(); ++i) {
		eliminate(cell, cell.possibles[i]);
    }
}

void Solver::eliminate(Cell& cell, string& digit) {
	//cout << "eliminate is getting called with s=" << s << " and d=" << d << endl;
	if(cell.has(digit)){
		return;
	}

	cell.remove(digit);

	//(1) If a square s is reduced to one value d2, then eliminate d2 from the peers.
	if(cell.possibles.empty()){
		return;
    // if there's only one, 
	} else if(cell.possibles.size() == 1){
        
		for (auto& sq_set : data_.peers[cell.key]){
            for (auto& s: sq_set) {
                std::cout << s << std::endl;
            }
                       
			// eliminate(cell, sq2);
		}
	}

	//(2) If a unit u is reduced to only one place for a value d, then put it there.
	for(int i = 0; i < 3; ++i){
		vector<string> dplaces = {};
		vector<string> u = str.units[s][i];
		for(int j = 0; j < 9; ++j){
			if(get<0>(values)[u[j]].find(d) != string::npos){
						string temp = u[j];
						dplaces.push_back(temp);}}
		if(dplaces.size() == 0){
			get<1>(values) = false;
			return;
		}

		else if(dplaces.size() == 1){
			// cout << "assign is getting called within eliminate" << endl;
			assign(values,dplaces[0],d);
			// if(!get<1>(values)){return;}}}
// 	return;
}

void


} // namespace Sudoku