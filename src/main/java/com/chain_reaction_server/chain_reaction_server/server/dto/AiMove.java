package com.chain_reaction_server.chain_reaction_server.server.dto;

public class AiMove {
    private final int row;
    private final int col;

    public AiMove(int row, int col) {
        this.row = row;
        this.col = col;
    }

    public int getRow() {
        return row;
    }
    public int getCol() {
        return col;
    }
}
