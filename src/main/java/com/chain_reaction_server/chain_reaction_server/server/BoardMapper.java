package com.chain_reaction_server.chain_reaction_server.server;

import com.chain_reaction_server.chain_reaction_server.server.dto.BoardDto;
import com.chain_reaction_server.chain_reaction_server.server.dto.CellDto;

import java.util.ArrayList;
import java.util.List;

public class BoardMapper {

    public static Board fromDto(BoardDto boardDto) {
        int rows = boardDto.getCells().size();
        int cols = boardDto.getCells().get(0).size();
        List<List<Cell>> boardCells = new ArrayList<>();
        for (int r = 0; r < rows; r++) {
            boardCells.add(new ArrayList<>());
            for (int c = 0; c < cols; c++) {
                CellDto cellDto = boardDto.getCells().get(r).get(c);
                Cell cell = new Cell(cellDto.getCount(), cellDto.getColor());
                boardCells.get(r).add(cell);
            }
        }
        return new Board(rows, cols, boardCells);
    }
}
