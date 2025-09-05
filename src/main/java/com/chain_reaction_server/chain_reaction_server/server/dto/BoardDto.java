package com.chain_reaction_server.chain_reaction_server.server.dto;

import java.util.List;

public class BoardDto {
    private List<List<CellDto>> cells;

    public List<List<CellDto>> getCells() {
        return cells;
    }
}
