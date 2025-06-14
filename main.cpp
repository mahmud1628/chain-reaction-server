#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Board.hpp"
#define ROWS 9
#define COLS 6
using namespace std;

int main() {
    ifstream inputFile("../gameState.txt");
    if (!inputFile) {
        cerr << "Error opening file." << endl;
        return 1;
    }
    vector<vector<cell>> cells(ROWS, vector<cell>(COLS));
    string line;
    getline(inputFile, line); // Read the first line (header)
    cout << line << endl;
    if(line != "Human Move:") {
        cerr << "Invalid file format or not a human move." << endl;
        return 1;
    }
    for(int i = 0; i < ROWS; i++) {
        getline(inputFile, line);
        if(line.empty()) {
            cerr << "Error: Not enough rows" << endl;
            return 1;
        }
        stringstream ss(line);
        string cell_data;
        for(int j = 0; j < COLS; j++) {
            ss >> cell_data;
            if(cell_data.empty()) {
                cerr << "Error: Not enough columns in row " << i + 1 << endl;
                return 1;
            }
            cells[i][j].set_count(cell_data[0] - '0'); // Convert char to int
            if(cell_data.size() == 2)
                cells[i][j].set_color(cell_data[1]); // Set color
        }
    }
    inputFile.close();

    Board board(ROWS, COLS);
    board.set_board(cells);
    return 0;
}