package com.chain_reaction_server.chain_reaction_server.server;

public class Cell {
    private int count;
    private char color;
    private int cell_type;

    public Cell() {
        this.count = 0;
        this.color = '\0';
    }

    public Cell(int count, char color) {
        this.count = count;
        this.color = color;
    }

    public Cell(int count, char color, int cell_type) {
        this.count = count;
        this.color = color;
        this.cell_type = cell_type;
    }

    public int getCount() {
        return count;
    }

    public void setCount(int count) {
        this.count = count;
    }

    public char getColor() {
        return color;
    }

    public void setColor(char color) {
        this.color = color;
    }

    public int getCell_type() {
        return cell_type;
    }

    public void setCell_type(int cell_type) {
        this.cell_type = cell_type;
    }
}
