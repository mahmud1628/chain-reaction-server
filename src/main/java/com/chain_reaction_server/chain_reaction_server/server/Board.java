package com.chain_reaction_server.chain_reaction_server.server;

import com.chain_reaction_server.chain_reaction_server.server.dto.AiMove;

import java.util.ArrayList;
import java.util.List;

public class Board {
    private final int rows;
    private final int cols;
    private final List<List<Cell>> cells;

    public Board(int rows, int cols, List<List<Cell>> cells) {
        this.rows = rows;
        this.cols = cols;
        this.cells = cells;

        // Initialize cell types
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (isCornerCell(i, j)) {
                    cells.get(i).get(j).setCell_type(Constants.CORNER_CELL);
                } else if (isEdgeCell(i, j)) {
                    cells.get(i).get(j).setCell_type(Constants.EDGE_CELL);
                } else {
                    cells.get(i).get(j).setCell_type(Constants.NORMAL_CELL);
                }
            }
        }
    }

    public Board(Board other) {
        this.rows = other.rows;
        this.cols = other.cols;
        this.cells = new ArrayList<>();
        for (int i = 0; i < rows; i++) {
            List<Cell> row = new ArrayList<>();
            for (int j = 0; j < cols; j++) {
                Cell otherCell = other.cells.get(i).get(j);
                Cell newCell = new Cell(otherCell.getCount(), otherCell.getColor(), otherCell.getCell_type());
                row.add(newCell);
            }
            this.cells.add(row);
        }
    }

    public void printBoard() {
        for (List<Cell> row : cells) {
            for (Cell cell : row) {
                System.out.print("[" + cell.getCount() + "," + (cell.getColor() == '\0' ? " " : cell.getColor()) + "] ");
            }
            System.out.println();
        }
    }

    public AiMove getAiMove()
    {
        int bestValue = Constants.INT_MIN;
        Coord bestMove = new Coord(-1, -1);

        List<Coord> validMoves = getValidMoves(Constants.AI);
        // System.out.println("AI Valid Moves: " + validMoves.size());
        for (var move : validMoves)
        {
            Board new_board = new Board(this);
            new_board.updateCell(move, Constants.AI);
            int d = new_board.depth(); // Adjust depth based on the game state
            int moveValue = new_board.minimax(d - 1, false, bestValue, Constants.INT_MAX);
            if (moveValue > bestValue)
            {
                bestValue = moveValue;
                bestMove = move;
            }
        }
        if(bestMove.row == -1) {
            bestMove = validMoves.getFirst(); // If no best moves, return the first valid move
        }
        return new AiMove(bestMove.row, bestMove.col);
    }

    private boolean isValidMove(int row, int col, char color)
    {
        if (cells.get(row).get(col).getCount() == 0)
            return true; // can place an orb in an empty cell
        return cells.get(row).get(col).getColor() == color; // can place an orb in a cell of the same color
    }

    private boolean isCornerCell(int row, int col)
    {
        boolean upper_left_corner = row == 0 && col == 0;
        boolean upper_right_corner = row == 0 && col == this.cols - 1;
        boolean lower_left_corner = row == this.rows - 1 && col == 0;
        boolean lower_right_corner = row == this.rows - 1 && col == this.cols - 1;
        return upper_left_corner || upper_right_corner || lower_right_corner || lower_left_corner;
    }

    private boolean isEdgeCell(int row, int col)
    {
        boolean is_top_row = row == 0;
        boolean is_bottom_row = row == this.rows - 1;
        boolean is_left_column = col == 0;
        boolean is_right_column = col == this.cols - 1;
        return is_top_row || is_bottom_row || is_left_column || is_right_column;
    }

    private int getCriticalMass(int row, int col)
    {
        int cellType = cells.get(row).get(col).getCell_type();

        if(cellType == Constants.CORNER_CELL) return Constants.CORNER_CRITICAL_MASS;
        if(cellType == Constants.EDGE_CELL) return Constants.EDGE_CRITICAL_MASS;
        return Constants.NORMAL_CRITICAL_MASS;
    }

    private List<Coord> getValidMoves(char color)
    {
        List<Coord> valid_moves = new ArrayList<>();
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                if (isValidMove(i, j, color))
                    valid_moves.add(new Coord(i, j));

        return valid_moves;
    }

    private void updateCell(Coord move, char currentPlayer)
    {
        int row = move.row;
        int col = move.col;
        if (!isValidMove(row, col, currentPlayer))
        {
            System.out.println("Invalid Move at (" + row + "," + col + ") by player " + currentPlayer);
            return; // If the move is invalid, do nothing
        }
        int cellCount = cells.get(row).get(col).getCount();
        if (cellCount == 0)
            cells.get(row).get(col).setColor(currentPlayer);
        cells.get(row).get(col).setCount(cellCount + 1);
        if (cellCount + 1 >= getCriticalMass(row, col))
            generateExplosion(row, col, currentPlayer);
    }

    private void generateExplosion(int startRow, int startCol, char currentPlayer)
    {
        List<Coord> indicesOfCurrentExplodingCells = new ArrayList<>();
        indicesOfCurrentExplodingCells.add(new Coord(startRow, startCol));
        char opponentPlayer = (currentPlayer == Constants.HUMAN) ? Constants.AI : Constants.HUMAN;
        int opponentOrbs = countOfOrbs(opponentPlayer);
        int explosionCount = 0;

        while (!indicesOfCurrentExplodingCells.isEmpty())
        {
            explosionCount++;
            if(explosionCount > 20) break;
            List<Coord> indicesOfNextExplodingCells = new ArrayList<>();
            for (var index : indicesOfCurrentExplodingCells)
            {
                int row = index.row;
                int col = index.col;

                // Reset the cell count
                this.cells.get(row).get(col).setCount(0);
                this.cells.get(row).get(col).setColor('\0'); // Reset color

                List<Coord> indicesOfOrthogonalCells = new ArrayList<>();
                indicesOfOrthogonalCells.add(new Coord(row - 1, col)); // Up
                indicesOfOrthogonalCells.add(new Coord(row + 1, col)); // Down
                indicesOfOrthogonalCells.add(new Coord(row, col - 1)); // Left
                indicesOfOrthogonalCells.add(new Coord(row, col + 1)); // Right


                for (var orthogonalIndex : indicesOfOrthogonalCells)
                {
                    int orthogonalRow = orthogonalIndex.row;
                    int orthogonalCol = orthogonalIndex.col;

                    boolean isValidRow = orthogonalRow >= 0 && orthogonalRow < this.rows;
                    boolean isValidCol = orthogonalCol >= 0 && orthogonalCol < this.cols;
                    boolean isValidIndex = isValidRow && isValidCol;
                    if (isValidIndex)
                    {
                        int cellCount = this.cells.get(orthogonalRow).get(orthogonalCol).getCount();
                        char cellColor = this.cells.get(orthogonalRow).get(orthogonalCol).getColor();
                        if (cellColor == opponentPlayer)
                            opponentOrbs -= cellCount;

                        this.cells.get(orthogonalRow).get(orthogonalCol).setCount(cellCount + 1);
                        if (cellCount + 1 >= getCriticalMass(orthogonalRow, orthogonalCol))
                            indicesOfNextExplodingCells.add(new Coord(orthogonalRow, orthogonalCol)); // Add to next explosion

                        this.cells.get(orthogonalRow).get(orthogonalCol).setColor(currentPlayer);
                    }
                }
            }
            indicesOfCurrentExplodingCells = indicesOfNextExplodingCells; // Move to the next explosion
            if (opponentOrbs <= 0)
                return; // terminal state reached
        }
    }

    private int countOfOrbs(char currentPlayer)
    {
        int count = 0;
        for (int i = 0; i < this.rows; i++)
        {
            for (int j = 0; j < this.cols; j++)
            {
                if (cells.get(i).get(j).getColor() == currentPlayer)
                {
                    count += cells.get(i).get(j).getCount(); // Count the opponent's cells
                }
            }
        }

        return count;
    }

    private int orbDifference()
    {
        int aiOrbs = countOfOrbs(Constants.AI);
        int humanOrbs = countOfOrbs(Constants.HUMAN);
        return aiOrbs - humanOrbs;
    }

    private int positionalAdvantageByCells()
    {
        int advantage = 0; // respect to the AI player
        for(int i =0; i < this.rows; i++)
        {
            for(int j = 0; j < this.cols; j++)
            {
                char cellColor = this.cells.get(i).get(j).getColor();
                if(cellColor == '\0') continue; // Skip empty cells
                int cellType = this.cells.get(i).get(j).getCell_type();
                int cellAdvantage;
                if(cellType == Constants.CORNER_CELL || cellType == Constants.EDGE_CELL) cellAdvantage = 2;
                else cellAdvantage = 1;

                if(cellColor == Constants.AI) advantage += cellAdvantage;
                else if(cellColor == Constants.HUMAN) advantage -= cellAdvantage;
            }
        }
        return advantage;
    }

    private int positionalAdvantageByOrbs()
    {
        int advantage = 0; // respect to the AI player
        for(int i = 0; i < this.rows; i++)
        {
            for(int j = 0; j < this.cols; j++)
            {
                char cellColor = this.cells.get(i).get(j).getColor();
                int cellCount = this.cells.get(i).get(j).getCount();
                int cellType = this.cells.get(i).get(j).getCell_type();
                if(cellCount == 0) continue; // Skip empty cells

                int cellAdvantage;
                if(cellType == Constants.CORNER_CELL || cellType == Constants.EDGE_CELL) cellAdvantage = 2;
                else cellAdvantage = 1;

                if(cellColor == Constants.AI) advantage += cellCount * cellAdvantage;
                else advantage -= cellCount * cellAdvantage;
            }
        }
        return advantage;
    }

    private int criticalCellDifference()
    {
        int aiCriticalCells = 0;
        int humanCriticalCells = 0;

        for (int i = 0; i < this.rows; i++)
        {
            for (int j = 0; j < this.cols; j++)
            {
                char cellColor = this.cells.get(i).get(j).getColor();
                int cellCount = this.cells.get(i).get(j).getCount();
                int criticalMass = getCriticalMass(i, j);

                if(cellCount == criticalMass - 1) {
                    if(cellColor == Constants.AI) aiCriticalCells++;
                    else if(cellColor == Constants.HUMAN) humanCriticalCells++;
                }

            }
        }
        return aiCriticalCells - humanCriticalCells;
    }

    private int adjacencyAdvantage()
    {
        int advantage = 0; // respect to the AI player
        for(int i = 0; i < this.rows; i++)
        {
            for(int j =0; j< this.cols; j++)
            {
                int cellAdvantage = 0, cellDisadvantage = 0;
                int cellCount = this.cells.get(i).get(j).getCount();
                if(cellCount == 0) continue;
                char cellColor = this.cells.get(i).get(j).getColor();
                int criticalMass = getCriticalMass(i, j);

                List<Coord> indicesOfOrthogonalCells = new ArrayList<>();
                indicesOfOrthogonalCells.add(new Coord(i - 1, j)); // Up
                indicesOfOrthogonalCells.add(new Coord(i + 1, j)); // Down
                indicesOfOrthogonalCells.add(new Coord(i, j - 1)); // Left
                indicesOfOrthogonalCells.add(new Coord(i, j + 1)); // Right

                if(cellColor == Constants.AI) {
                    cellAdvantage += cellCount;
                    for(var index : indicesOfOrthogonalCells){
                        int row = index.row, col = index.col;
                        if(isValidIndex(row, col)) {
                            if(cells.get(row).get(col).getColor() == Constants.AI) {
                                cellAdvantage += (cellCount == criticalMass - 1) ? 2 : 1;
                            } else if(cells.get(row).get(col).getColor() == Constants.HUMAN) {
                                cellDisadvantage  += 1;
                            }
                        }
                    }
                }
                else {
                    cellDisadvantage += cellCount;
                    for(var index : indicesOfOrthogonalCells){
                        int row = index.row, col = index.col;
                        if(isValidIndex(row, col)) {
                            if(cells.get(row).get(col).getColor() == Constants.AI) {
                                cellDisadvantage += (cellCount == criticalMass - 1) ? 2 : 1;
                            } else if(cells.get(row).get(col).getColor() == Constants.HUMAN) {
                                cellAdvantage  += 1;
                            }
                        }
                    }
                }
                advantage += cellAdvantage - cellDisadvantage;
            }
        }
        return advantage;
    }

    private boolean isValidIndex(int row, int col)
    {
        boolean isValidRow = row >= 0 && row < this.rows;
        boolean isValidCol = col >= 0 && col < this.cols;
        return isValidRow && isValidCol;
    }

    private int evaluateBoard()
    {
        // int score = orb_difference();
        return adjacencyAdvantage();
    }

    private boolean isTerminalState()
    {
        boolean humanHasOrbs = false;
        boolean aiHasOrbs = false;
        for (int i = 0; i < this.rows; i++)
        {
            for (int j = 0; j < this.cols; j++)
            {
                char cellColor = this.cells.get(i).get(j).getColor();
                if(cellColor == Constants.HUMAN) humanHasOrbs = true;
                else if(cellColor == Constants.AI) aiHasOrbs = true;

                if (humanHasOrbs && aiHasOrbs) return false; // Both players have orbs, not a terminal state
            }
        }
        return true; // One or both players have no orbs, terminal state
    }

    private boolean isWinningState(char player)
    {
        char opponent = (player == Constants.HUMAN) ? Constants.AI : Constants.HUMAN;
        for(int i =0; i < this.rows; i++)
        {
            for(int j =0; j < this.cols; j++)
            {
                if(this.cells.get(i).get(j).getColor() == opponent)
                {
                    return false;
                }
            }
        }
        return true;
    }

    private int depth()
    {
        int totalOrbs = countOfOrbs(Constants.HUMAN) + countOfOrbs(Constants.AI);
        if (totalOrbs <= 20)
            return 3;
        else
            return 2;
    }

    private int minimax(int depth, boolean isMaximizingPlayer, int alpha, int beta)
    {
        if(isWinningState(Constants.AI)) return Constants.INT_MAX;
        if(isWinningState(Constants.HUMAN)) return Constants.INT_MIN;

        if (depth == 0) return this.evaluateBoard();

        if (isMaximizingPlayer) // for AI player
        {
            int maxEval = Constants.INT_MIN;
            List<Coord> validMoves = getValidMoves(Constants.AI);
            for (var move : validMoves)
            {
                Board new_board = new Board(this);
                new_board.updateCell(move, Constants.AI);
                int eval = new_board.minimax(depth - 1, false, alpha, beta);
                maxEval = Math.max(maxEval, eval);
                alpha = Math.max(alpha, eval);
                if (beta <= alpha)
                {
                    break; // Stop searching if the current branch is worse than the best found so far
                }
            }
            return maxEval;
        }
        else // for human player
        {
            int minEval = Constants.INT_MAX;
            List<Coord> validMoves = getValidMoves(Constants.HUMAN);
            for (var move : validMoves)
            {
                Board new_board = new Board(this);
                new_board.updateCell(move, Constants.HUMAN);
                int eval = new_board.minimax(depth - 1, true, alpha, beta);
                minEval = Math.min(minEval, eval);
                beta = Math.min(beta, eval);
                if (beta <= alpha)
                {
                    break; // Stop searching if the current branch is worse than the best found so far
                }
            }
            return minEval;
        }
    }
}
