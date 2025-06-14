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

  exec("main.exe", (error, stdout, stderr) => {
    if (error) {
      console.error(`Error executing C++ program: ${error.message}`);
      return res.status(500).json({ error: "C++ execution failed" });
    }
    if (stderr) {
      console.error(`C++ stderr: ${stderr}`);
    }

    console.log(`C++ stdout: ${stdout}`);

    // Now that C++ finished, we can safely read the updated file
    const move = get_ai_move(board);  // compare previous board with updated one

    res.json({ row: move.row, col: move.col });
  });
});


app.listen(port, () => {
  console.log(`Server is listening at http://localhost:${port}`);
});
