#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Board.hpp"
#include <chrono>
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
    // auto start = chrono::high_resolution_clock::now();
    // pair<int, pair<int, int>> result = board.get_best_move(AI);
    pair<int, pair<int, int>> result = board.get_best_move(HUMAN); // for ai vs ai
    // auto end = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    int  best_value = result.first;
    int best_row = result.second.first;
    int best_col = result.second.second;
    
    cells[best_row][best_col].set_count(cells[best_row][best_col].get_count() + 1);
    cells[best_row][best_col].set_color(AI);

    ofstream outputFile("../gameState.txt");
    if (!outputFile) {
        cerr << "Error opening file for writing." << endl;
        return 1;
    }
    outputFile << "AI Move:" << endl;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if(cells[i][j].get_color() != '\0')
                outputFile << cells[i][j].get_count() << cells[i][j].get_color() << " ";
            else 
                outputFile << cells[i][j].get_count() << " "; // Only count if color is not set
        }
        outputFile << endl;
    }
    outputFile.close();
    // cout << "AI move saved to gameState.txt" << endl;
    cout << best_value << " " << best_row << " " << best_col << endl;


    // ofstream logFile("log.txt", ios::app); // for experimental purposes
    // if (!logFile) {
    //     cerr << "Error opening log file." << endl;
    //     return 1;
    // }
    // logFile << duration.count() << " ms" << endl;
    return 0;
}