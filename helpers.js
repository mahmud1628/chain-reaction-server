const fs = require('fs');
const path = require('path');
const file_path = path.join(__dirname, '../gameState.txt');

const write_game_state = (board) => {
    let data = 'Human Move:\n';
    for(let i = 0; i < board.length; i++) {
        for(let j = 0; j < board[i].length; j++) {
            let cell = board[i][j];
            if(cell.count === 0 && cell.color === null) {
                data += '0 ';
            }
            else {
                data += `${cell.count}${cell.color} `;
            }
        }
        data += '\n';
    }
    fs.writeFileSync(file_path, data, 'utf8', (err) => {
        if (err) {
            console.error('Error writing to file:', err);
        } else {
            console.log('Game state written successfully');
        }
    });
}

const get_ai_move = (board) => {
    const file_data = fs.readFileSync(file_path, 'utf8');
    const lines = file_data.split('\n').slice(1); // Skip "AI Move:"
    for (let i = 0; i < board.length; i++) {
        if (!lines[i]) continue;
        const cells = lines[i].trim().split(' ');
        for (let j = 0; j < board[i].length; j++) {
            let cell = board[i][j];
            let file_cell = cells[j];
            let expected = (cell.count === 0 && cell.color === null) ? '0' : `${cell.count}${cell.color}`;
            if (file_cell !== expected) {
                return { row: i, col: j };
            }
        }
    }
    return null;
}


exports.write_game_state = write_game_state;
exports.get_ai_move = get_ai_move;