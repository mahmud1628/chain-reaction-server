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

#define CORNER_CRITICAL_MASS 2
#define EDGE_CRITICAL_MASS 3
#define NORMAL_CRITICAL_MASS 4
#define CORNER_CELL 1
#define EDGE_CELL 2
#define NORMAL_CELL 3
using namespace std;

class cell
{
private:
    int count;
    char color;
    int cell_type;

public:
    cell() : count(0), color('\0') {}
    cell(int c, char col) : count(c), color(col), cell_type(0) {}
    cell(int c, char col, int type) : count(c), color(col), cell_type(type) {}
    int get_count() const { return count; }
    char get_color() const { return color; }
    int get_cell_type() const { return cell_type; }
    void set_cell_type(int type) { cell_type = type; }
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
    int evaluate_board();
    bool is_terminal_state();
    int depth();
    bool is_winning_state(char player);
    bool is_corner_cell(int row, int col);
    bool is_edge_cell(int row, int col);
    bool is_valid_index(int row, int col);

    // heuristics
    int orb_difference();
    int positional_advantage_by_cells();
    int positional_advantage_by_orbs();
    int critical_cell_difference();
    int adjacency_advantage();

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
    cout << "Board set successfully." << endl;
    for(int i = 0; i < this->rows; i++)
    {
        for(int j = 0; j < this->cols; j++)
        {
            if(this->is_corner_cell(i, j)) this->cells[i][j].set_cell_type(CORNER_CELL);
            else if(this->is_edge_cell(i, j)) this->cells[i][j].set_cell_type(EDGE_CELL);
            else this->cells[i][j].set_cell_type(NORMAL_CELL);
        }
    }

}

bool Board::is_valid_move(int row, int col, char color)
{
    if (cells[row][col].get_count() == 0)
        return true; // can place an orb in an empty cell
    if (cells[row][col].get_color() == color)
        return true; // can place an orb in a cell of the same color
    return false;
}

bool Board::is_corner_cell(int row, int col)
{
    bool upper_left_corner = row == 0 && col == 0;
    bool upper_right_corner = row == 0 && col == this->cols - 1;
    bool lower_left_corner = row == this->rows - 1 && col == 0;
    bool lower_right_corner = row == this->rows - 1 && col == this->cols - 1;
    bool is_corner = upper_left_corner || upper_right_corner || lower_right_corner || lower_left_corner;
    return is_corner;
}

bool Board::is_edge_cell(int row, int col)
{
    bool is_top_row = row == 0;
    bool is_bottom_row = row == this->rows - 1;
    bool is_left_column = col == 0;
    bool is_right_column = col == this->cols - 1;
    bool is_edge = is_top_row || is_bottom_row || is_left_column || is_right_column;
    return is_edge;
}

int Board::get_critical_mass(int row, int col)
{
    int cell_type = this->cells[row][col].get_cell_type();

    if(cell_type == CORNER_CELL) return CORNER_CRITICAL_MASS;
    if(cell_type == EDGE_CELL) return EDGE_CRITICAL_MASS;
    return NORMAL_CRITICAL_MASS;
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

int Board::orb_difference()
{
    int ai_orbs = count_of_orbs(AI);
    int human_orbs = count_of_orbs(HUMAN);
    return ai_orbs - human_orbs;
}

int Board::positional_advantage_by_cells()
{
    int advantage = 0; // respect to the AI player
    for(int i =0; i < this->rows; i++)
    {
        for(int j = 0; j < this->cols; j++)
        {
            char cell_color = this->cells[i][j].get_color();
            if(cell_color == '\0') continue; // Skip empty cells
            int cell_type = this->cells[i][j].get_cell_type();
            int cell_advantage;
            if(cell_type == CORNER_CELL || cell_type == EDGE_CELL) cell_advantage = 2;
            else cell_advantage = 1;

            if(cell_color == AI) advantage += cell_advantage;
            else if(cell_color == HUMAN) advantage -= cell_advantage;
        }
    }
    return advantage;
}

int Board::positional_advantage_by_orbs()
{
    int advantage = 0; // respect to the AI player
    for(int i = 0; i < this->rows; i++)
    {
        for(int j = 0; j < this->cols; j++)
        {
            char cell_color = this->cells[i][j].get_color();
            int cell_count = this->cells[i][j].get_count();
            int cell_type = this->cells[i][j].get_cell_type();
            if(cell_count == 0) continue; // Skip empty cells

            int cell_advantage;
            if(cell_type == CORNER_CELL || cell_type == EDGE_CELL) cell_advantage = 2;
            else cell_advantage = 1;

            if(cell_color == AI) advantage += cell_count * cell_advantage;
            else advantage -= cell_count * cell_advantage;
        }
    }
    return advantage;
}

int Board::critical_cell_difference()
{
    int ai_critical_cells = 0;
    int human_critical_cells = 0;

    for (int i = 0; i < this->rows; i++)
    {
        for (int j = 0; j < this->cols; j++)
        {
            char cell_color = this->cells[i][j].get_color();
            int cell_count = this->cells[i][j].get_count();
            int critical_mass = get_critical_mass(i, j);

            if(cell_count == critical_mass - 1) {
                if(cell_color == AI) ai_critical_cells++;
                else if(cell_color == HUMAN) human_critical_cells++;
            }
            
        }
    }
    return ai_critical_cells - human_critical_cells;
}

int Board::adjacency_advantage() 
{
    int advantage = 0; // resprect to the AI player
    for(int i = 0; i < this->rows; i++)
    {
        for(int j =0; j< this->cols; j++)
        {
            int cell_advantage = 0, cell_disadvantage = 0;
            int cell_count = this->cells[i][j].get_count();
            if(cell_count == 0) continue;
            char cell_color = this->cells[i][j].get_color();
            int critical_mass = get_critical_mass(i, j);

            vector<Coord> indices_of_orthogonal_cells = {
                {i - 1, j}, // Up
                {i + 1, j}, // Down
                {i, j - 1}, // Left
                {i, j + 1}  // Right
            };

            if(cell_color == AI) {
                cell_advantage += cell_count;
                for(auto &index : indices_of_orthogonal_cells){
                    int row = index.first, col = index.second;
                    if(is_valid_index(row, col)) {
                        if(cells[row][col].get_color() == AI) {
                            cell_advantage += (cell_count == critical_mass - 1) ? 2 : 1;
                        } else if(cells[row][col].get_color() == HUMAN) {
                            cell_disadvantage  += 1;
                        }
                    }
                }
            }
            else {
                cell_disadvantage += cell_count;
                for(auto &index : indices_of_orthogonal_cells){
                    int row = index.first, col = index.second;
                    if(is_valid_index(row, col)) {
                        if(cells[row][col].get_color() == AI) {
                            cell_disadvantage += (cell_count == critical_mass - 1) ? 2 : 1;
                        } else if(cells[row][col].get_color() == HUMAN) {
                            cell_advantage  += 1;
                        }
                    }
                }
            }
                    advantage += cell_advantage - cell_disadvantage;
        }
    }
    return advantage;
}

bool Board::is_valid_index(int row, int col)
{
    bool is_valid_row = row >= 0 && row < this->rows;
    bool is_valid_col = col >= 0 && col < this->cols;
    return is_valid_row && is_valid_col;
}

int Board::evaluate_board()
{
    // int score = orb_difference();
    int score = adjacency_advantage();
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

    if (depth == 0) return evaluate_board();

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