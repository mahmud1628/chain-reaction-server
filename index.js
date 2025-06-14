const express = require("express");
const app = express();
const cors = require("cors");
const port = 3000;
const write_game_state = require("./helpers").write_game_state;
const { exec } = require("child_process");

app.use(express.json());
app.use(cors());

app.post("/ai-move", (req, res) => {
  const { board } = req.body;

  //console.log("Received board:", board);
  write_game_state(board);

  exec("test-ai.exe", (error, stdout, stderr) => {
    if (error) {
      console.error(`Error executing C++ program: ${error.message}`);
      return;
    }
    if (stderr) {
      console.error(`C++ stderr: ${stderr}`);
      return;
    }
    console.log(`C++ stdout: ${stdout}`);
  });

  

  res.json({ row: 3, col: 4 }); // Dummy response for now
});

app.listen(port, () => {
  console.log(`Server is listening at http://localhost:${port}`);
});
