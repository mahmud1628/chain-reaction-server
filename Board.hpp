#ifndef _BOARD_HPP
#define _BOARD_HPP

#include <iostream>
#include <vector>

#define Move pair<int, int> // Define Move as a pair of integers for row and column indices
#define HUMAN 'R'
#define AI 'B'
using namespace std;

class cell
{
private:
    int count;
    char color;

public:
    cell() : count(0), color('\0') {}               // Default constructor initializes count to 0 and color to null character
    cell(int c, char col) : count(c), color(col) {} // Parameterized constructor
    int get_count() const { return count; }         // Getter for count
    char get_color() const { return color; }        // Getter for color

    void set_count(int c) { count = c; }      // Setter for count
    void set_color(char col) { color = col; } // Setter for color
};

class Board
{
private:
    int rows;                   // Number of rows in the board
    int cols;                   // Number of columns in the board
    vector<vector<cell>> cells; // 2D vector to hold cells

    vector<Move> get_valid_moves(char color);
    bool is_valid_move(int row, int col, char color);
    bool update_board(Move move, char color);
    int get_critical_mass(int row, int col);
    void generate_explosion(int start_row, int start_col, char current_player);
    int count_of_orbs(char current_player);

public:
    Board(int rows, int cols) : rows(rows), cols(cols), cells(rows, vector<cell>(cols)) {}
    Board(const Board &other) : rows(other.rows), cols(other.cols), cells(other.cells) {}
    void set_board(const vector<vector<cell>> &new_cells);
};

void Board::set_board(const vector<vector<cell>> &new_cells)
{
    if (new_cells.size() != rows || new_cells[0].size() != cols)
    {
        cerr << "Error: New board dimensions do not match." << endl;
        return;
    }
    cells = new_cells; // Set the board to the new cells
}

bool Board::is_valid_move(int row, int col, char color)
{
    if (cells[row][col].get_count() == 0)
        return true; // can place an orb in an empty cell
    if (cells[row][col].get_color() == color)
        return true; // ai can place an orb in a blue cell
    return false;
}

int Board::get_critical_mass(int row, int col)
{
    bool upper_left_corner = row == 0 && col == 0;
    bool upper_right_corner = row == 0 && col == this->cols - 1;
    bool lower_left_corner = row == this->rows - 1 && col == 0;
    bool lower_right_corner = row == this->rows - 1 && col == this->cols - 1;

    if (
        upper_left_corner ||
        upper_right_corner ||
        lower_left_corner ||
        lower_right_corner)
    {
        return 2; // Critical mass for corners is 2
    }

    bool is_top_row = row == 0;
    bool is_bottom_row = row == this->rows - 1;
    bool is_left_column = col == 0;
    bool is_right_column = col == this->cols - 1;

    if (is_top_row || is_bottom_row || is_left_column || is_right_column)
    {
        return 3; // Critical mass for edges is 3
    }

    return 4; // Critical mass for all other cells is 4
}

vector<Move> Board::get_valid_moves(char color)
{
    vector<Move> valid_moves;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (is_valid_move(i, j, color))
            {
                valid_moves.push_back(Move(i, j)); // Add valid move to the list
            }
        }
    }
    return valid_moves; // Return the list of valid moves
}

bool Board::update_board(Move move, char current_player)
{
    int row = move.first;
    int col = move.second;
    if (!is_valid_move(row, col, current_player))
    {
        cerr << "Invalid move at (" << row << ", " << col << ") for color " << current_player << endl;
        return false; // If the move is invalid, do nothing
    }
    if (cells[row][col].get_count() == 0)
    {
        cells[row][col].set_color(current_player); // Set the color of the cell
    }
    cells[row][col].set_count(cells[row][col].get_count() + 1); // Increment the count of the cell
    if (cells[row][col].get_count() >= get_critical_mass(row, col))
    {
        generate_explosion(row, col, current_player);
    }
    return true;
}

void Board::generate_explosion(int start_row, int start_col, char current_player)
{
    vector<pair<int, int>> indices_of_current_exploding_cells;
    indices_of_current_exploding_cells.push_back(make_pair(start_row, start_col));
    char opponent_player = (current_player == HUMAN) ? AI : HUMAN;
    int opponent_orbs = count_of_orbs(opponent_player);

    while (!indices_of_current_exploding_cells.empty())
    {
        vector<pair<int, int>> indices_of_next_exploding_cells;
        for (auto &cell : indices_of_current_exploding_cells)
        {
            int row = cell.first;
            int col = cell.second;

            // Reset the cell count
            this->cells[row][col].set_count(0);
            this->cells[row][col].set_color('\0'); // Reset color

            vector<pair<int, int>> indices_of_orthogonal_cells = {
                {row - 1, col}, // Up
                {row + 1, col}, // Down
                {row, col - 1}, // Left
                {row, col + 1}  // Right
            };

            for (auto &orthogonal_index : indices_of_orthogonal_cells)
            {
                int orthogonal_row = orthogonal_index.first;
                int orthogonal_col = orthogonal_index.second;

                bool is_valid_row = orthogonal_row >= 0 && orthogonal_row < this->rows;
                bool is_valid_col = orthogonal_col >= 0 && orthogonal_col < this->cols;
                bool is_valid_index = is_valid_row && is_valid_col;
                if (is_valid_row)
                {
                    if (this->cells[orthogonal_row][orthogonal_col].get_color() == opponent_player)
                    {
                        opponent_orbs -= this->cells[orthogonal_row][orthogonal_col].get_count();
                    }
                    this->cells[orthogonal_row][orthogonal_col].set_count(
                        this->cells[orthogonal_row][orthogonal_col].get_count() + 1);
                    if (this->cells[orthogonal_row][orthogonal_col].get_count() >=
                        get_critical_mass(orthogonal_row, orthogonal_col))
                    {
                        indices_of_next_exploding_cells.push_back(
                            make_pair(orthogonal_row, orthogonal_col)); // Add to next explosion
                    }
                    this->cells[orthogonal_row][orthogonal_col].set_color(current_player);
                }
            }
        }
        indices_of_current_exploding_cells = indices_of_next_exploding_cells; // Move to the next explosion
        if (opponent_orbs <= 0)
        {
            return;
        }
    }
}

int Board::count_of_orbs(char current_player)
{
    int count = 0;
    for (int i = 0; i < this->rows; i++)
    {
        for (int j = 0; j < this->cols; j++)
        {
            if (cells[i][j].get_color() == current_player)
            {
                count += cells[i][j].get_count(); // Count the opponent's cells
            }
        }
    }
    return count;
}

#endif // _BOARD_HPP