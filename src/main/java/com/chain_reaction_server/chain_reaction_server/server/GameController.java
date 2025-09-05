package com.chain_reaction_server.chain_reaction_server.server;

import com.chain_reaction_server.chain_reaction_server.server.dto.AiMove;
import com.chain_reaction_server.chain_reaction_server.server.dto.BoardDto;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class GameController {


    @Autowired
    private GameService gameService;

    @PostMapping("/ai-move")
    public ResponseEntity<AiMove> getAiMove(@RequestBody BoardDto boardDto) {
        try {
            AiMove aiMove = gameService.getAiMove(boardDto);
            return ResponseEntity.ok(aiMove);
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).build();
        }
    }
}
