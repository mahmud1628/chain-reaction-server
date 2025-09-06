package com.chain_reaction_server.chain_reaction_server.server;

import com.chain_reaction_server.chain_reaction_server.server.dto.AiMove;
import com.chain_reaction_server.chain_reaction_server.server.dto.BoardDto;
import org.springframework.stereotype.Service;

@Service
public class GameService {
    public AiMove getAiMove(BoardDto boardDto) {
        Board board = BoardMapper.fromDto(boardDto);
        // board.printBoard();
        try {
            return board.getAiMove();
        } catch(Exception e) {
            throw new RuntimeException("Error getting AI move", e);
        }
    }
}
