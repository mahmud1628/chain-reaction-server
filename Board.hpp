#ifndef _BOARD_HPP
#define _BOARD_HPP

#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

#define Move pair<int, int>
#define Coord pair<int, int>
#define HUMAN 'R' // red player
#define AI 'B'    // blue player
#define INT_MAX std::numeric_limits<int>::max()
#define INT_MIN std::numeric_limits<int>::min()
using namespace std;

class cell
{
private:
    int count;
    char color;

public:
    cell() : count(0), color('\0') {}
    cell(int c, char col) : count(c), color(col) {}
    int get_count() const { return count; }
    char get_color() const { return color; }
    void set_count(int c) { count = c; }
    void set_color(char col) { color = col; }
};

class Board
{
private:
    int rows;                   // Number of rows in the board
    int cols;                   // Number of columns in the board
    vector<vector<cell>> cells; // 2D vector to hold cells

    vector<Move> get_valid_moves(char color);
    bool is_valid_move(int row, int col, char color);
    int get_critical_mass(int row, int col);
    void generate_explosion(int start_row, int start_col, char current_player);
    int count_of_orbs(char current_player);
    bool update_cell(Move move, char current_player);
    int minimax(int depth, bool is_maximizing_player, int alpha, int beta);
    int evaluate_board(char player);
    bool is_terminal_state();
    int depth();
    bool is_winning_state(char player);

    // heuristics
    int orb_difference(char player);

public:
    Board(int rows, int cols) : rows(rows), cols(cols), cells(rows, vector<cell>(cols)) {}
    Board(const Board &other) : rows(other.rows), cols(other.cols), cells(other.cells) {}
    void set_board(const vector<vector<cell>> &new_cells);
    void print_board();
    pair<int, Move> get_ai_move();
};

void Board::set_board(const vector<vector<cell>> &new_cells)
{
    if (new_cells.size() != rows || new_cells[0].size() != cols)
    {
        cerr << "Error: New board dimensions do not match." << endl;
        return;
    }
    this->cells = new_cells;
}

bool Board::is_valid_move(int row, int col, char color)
{
    if (cells[row][col].get_count() == 0)
        return true; // can place an orb in an empty cell
    if (cells[row][col].get_color() == color)
        return true; // can place an orb in a cell of the same color
    return false;
}

int Board::get_critical_mass(int row, int col)
{
    bool upper_left_corner = row == 0 && col == 0;
    bool upper_right_corner = row == 0 && col == this->cols - 1;
    bool lower_left_corner = row == this->rows - 1 && col == 0;
    bool lower_right_corner = row == this->rows - 1 && col == this->cols - 1;
    bool is_corner = upper_left_corner || upper_right_corner || lower_right_corner || lower_left_corner;

    if (is_corner)
    {
        return 2; // Critical mass for corners is 2
    }

    bool is_top_row = row == 0;
    bool is_bottom_row = row == this->rows - 1;
    bool is_left_column = col == 0;
    bool is_right_column = col == this->cols - 1;
    bool is_edge = is_top_row || is_bottom_row || is_left_column || is_right_column;

    if (is_edge)
    {
        return 3; // Critical mass for edges is 3
    }

    return 4; // Critical mass for all other cells is 4
}

vector<Move> Board::get_valid_moves(char color)
{
    vector<Move> valid_moves;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (is_valid_move(i, j, color))
                valid_moves.push_back(Move(i, j));

    return valid_moves;
}

bool Board::update_cell(Move move, char current_player)
{
    int row = move.first;
    int col = move.second;
    if (!is_valid_move(row, col, current_player))
    {
        cerr << "Invalid move at (" << row << ", " << col << ") for color " << current_player << endl;
        return false; // If the move is invalid, do nothing
    }
    int cell_count = cells[row][col].get_count();
    if (cell_count == 0)
        cells[row][col].set_color(current_player);
    cells[row][col].set_count(cell_count + 1);
    if (cell_count + 1 >= get_critical_mass(row, col))
        generate_explosion(row, col, current_player);
    return true;
}

void Board::generate_explosion(int start_row, int start_col, char current_player)
{
    vector<Coord> indices_of_current_exploding_cells;
    indices_of_current_exploding_cells.push_back(make_pair(start_row, start_col));
    char opponent_player = (current_player == HUMAN) ? AI : HUMAN;
    int opponent_orbs = count_of_orbs(opponent_player);
    int explosion_count = 0;

    while (!indices_of_current_exploding_cells.empty())
    {
        explosion_count++;
        if(explosion_count > 20) break;
        vector<Coord> indices_of_next_exploding_cells;
        for (auto &index : indices_of_current_exploding_cells)
        {
            int row = index.first;
            int col = index.second;

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
                if (is_valid_index)
                {
                    int cell_count = this->cells[orthogonal_row][orthogonal_col].get_count();
                    char cell_color = this->cells[orthogonal_row][orthogonal_col].get_color();
                    if (cell_color == opponent_player)
                        opponent_orbs -= cell_count;

                    this->cells[orthogonal_row][orthogonal_col].set_count(cell_count + 1);
                    if (cell_count + 1 >= get_critical_mass(orthogonal_row, orthogonal_col))
                        indices_of_next_exploding_cells.push_back(
                            make_pair(orthogonal_row, orthogonal_col)); // Add to next explosion

                    this->cells[orthogonal_row][orthogonal_col].set_color(current_player);
                }
            }
        }
        indices_of_current_exploding_cells = indices_of_next_exploding_cells; // Move to the next explosion
        if (opponent_orbs <= 0)
            return; // terminal state reached
    }
    return; // end of while loop
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

void Board::print_board()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cout << cells[i][j].get_count() << cells[i][j].get_color() << "\t";
        }
        cout << endl; // New line after each row
    }
}

int Board::orb_difference(char player)
{
    int score = 0;
    for (int i = 0; i < this->rows; i++)
    {
        for (int j = 0; j < this->cols; j++)
        {
            char cell_color = this->cells[i][j].get_color();
            int cell_count = this->cells[i][j].get_count();
            if (cell_color == player)
            {
                score += cell_count; // Add count for player's cells
            }
            else if (cell_color != '\0')
            {                        // If the cell is occupied by the opponent
                score -= cell_count; // Subtract count for opponent's cells
            }
        }
    }
    return score;
}

int Board::evaluate_board(char player)
{
    int score = orb_difference(player);
    return score;
}

bool Board::is_terminal_state()
{
    bool human_has_orbs = false;
    bool ai_has_orbs = false;
    for (int i = 0; i < this->rows; i++)
    {
        for (int j = 0; j < this->cols; j++)
        {
            char cell_color = this->cells[i][j].get_color();
            if(cell_color == HUMAN) human_has_orbs = true;
            else if(cell_color == AI) ai_has_orbs = true;

            if (human_has_orbs && ai_has_orbs) return false; // Both players have orbs, not a terminal state
        }
    }
    return true; // One or both players have no orbs, terminal state
}

bool Board::is_winning_state(char player)
{
    char opponent = (player == HUMAN) ? AI : HUMAN;
    for(int i =0; i < this->rows; i++)
    {
        for(int j =0; j < this->cols; j++)
        {
            if(this->cells[i][j].get_color() == opponent)
            {
                return false;
            }
        }
    }
    return true;
}

int Board::minimax(int depth, bool is_maximizing_player, int alpha, int beta)
{
    if(is_winning_state(AI)) return INT_MAX;
    if(is_winning_state(HUMAN)) return INT_MIN;

    if (depth == 0) return evaluate_board(AI);

    //cout << "Minimax depth: " << depth << ", is_maximizing_player: " << is_maximizing_player << endl;

    if (is_maximizing_player) // for AI player
    {
        int max_eval = INT_MIN;
        vector<Move> valid_moves = get_valid_moves(AI);
        for (const Move &move : valid_moves)
        {
            Board new_board = *this;
            new_board.update_cell(move, AI);
            int eval = new_board.minimax(depth - 1, false, alpha, beta);
            max_eval = max(max_eval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha)
            {
                break; // Stop searching if the current branch is worse than the best found so far
            }
        }
        return max_eval;
    }
    else // for human player
    {
        int min_eval = INT_MAX;
        vector<Move> valid_moves = get_valid_moves(HUMAN);
        for (const Move &move : valid_moves)
        {
            Board new_board = *this;
            new_board.update_cell(move, HUMAN);
            int eval = new_board.minimax(depth - 1, true, alpha, beta);
            min_eval = min(min_eval, eval);
            beta = min(beta, eval);
            if (beta <= alpha)
            {
                break; // Stop searching if the current branch is worse than the best found so far
            }
        }
        return min_eval;
    }
}

pair<int, Move> Board::get_ai_move()
{
    int best_value = INT_MIN;
    Move best_move = {-1, -1};

    vector<Move> valid_moves = get_valid_moves(AI);
    for (const Move &move : valid_moves)
    {
        // cout << "Evaluating move: (" << move.first << ", " << move.second << ") ";
        Board new_board = *this;
        new_board.update_cell(move, AI);
        int dpth = new_board.depth(); // Adjust depth based on the game state
        int move_value = new_board.minimax(dpth - 1, false, best_value, INT_MAX);
        //cout << "Move value: " << move_value << endl;
        if (move_value > best_value)
        {
            best_value = move_value;
            best_move = move;
        }
    }
    return make_pair(best_value, best_move);
}

int Board::depth()
{
    int total_orbs = count_of_orbs(HUMAN) + count_of_orbs(AI);
    if (total_orbs <= 10)
        return 5; // Early game
    else if (total_orbs <= 20)
        return 3; // Mid game
    // else if( total_orbs <= 30)
    //     return 3; // Late game
    else 
        return 2; // End game
}

#endif // _BOARD_HPP