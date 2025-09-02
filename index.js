const express = require("express");
const app = express();
const cors = require("cors");
const port = 3000;
const write_game_state = require("./helpers").write_game_state;
const get_ai_move = require("./helpers").get_ai_move;
const { exec } = require("child_process");

app.use(express.json());
app.use(cors());

app.post("/ai-move", (req, res) => {
  const { board } = req.body;

  write_game_state(board); // write the original board to file

  exec("./main", (error, stdout, stderr) => {
    if (error) {
      console.error(`Error executing C++ program: ${error.message}`);
      return res.status(500).json({ error: "C++ execution failed" });
    }
    if (stderr) {
      console.error(`C++ stderr: ${stderr}`);
    }

    // const lines = stdout.trim().split("\n"); // to use c++ output directly to get the best move
    // const [val, row, col] = lines[lines.length - 1].split(" ").map(Number);
    // console.log("Best value:", val);
    // console.log(`AI Move: Row ${row}, Col ${col}`);
    try {
      const aiMove = get_ai_move(board); // get the AI move from the file
      const { row, col } = aiMove;
      console.log(`AI move is: Row ${row}, Col ${col}`);
      res.json({ row: row, col: col });
    } catch (err) {
      console.error("Error getting AI move:", err);
      return res.status(500).json({ error: "Failed to get AI move" });
    }

  });
});

app.listen(port, () => {
  console.log(`Server is listening at http://localhost:${port}`);
});
