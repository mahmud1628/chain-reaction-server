package com.chain_reaction_server.chain_reaction_server.server.dto;

public class CellDto {
    private final int count;
    private final char color;

    public CellDto() {
        this.count = 0;
        this.color = '\0';
    }

    public CellDto(int count, char color) {
        this.count = count;
        this.color = color;
    }

    public int getCount() {
        return count;
    }

    public char getColor() {
        return color;
    }
}
