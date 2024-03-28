const express = require('express');
const app = express();
const path = require('path');

//  homepage route
app.get('/', (req, res) => {
    res.sendFile(path.resolve(__dirname + '/index.html'));
});

// route to play the game
app.get('/:name', (req, res) => {
    const userNumber = parseInt(req.query.num);
    const userName = req.params.name;
    const serverNumber = Math.floor(Math.random() * 10) + 1;

    if (userNumber && typeof userName === 'string' && !isNaN(userNumber) && Number.isInteger(userNumber) && userNumber >= 1 && userNumber <= 10) {
        if (userNumber === serverNumber) {
            res.send(`Congratulations ${userName}! You're a winner!`);
        } else {
            res.send(`Sorry ${userName}, you picked ${userNumber}, but the server picked ${serverNumber}. Better luck next time!`);
        }
    } else {
        res.send(`Wrong input, ${userName}. It appears that you're a dumbass and can't follow simple instructions.`);
    }
});

// Catch-all route
app.get('/*', (req, res) => {
    res.status(404).redirect('/');
});

app.listen(3000, () => console.log('Server is active on http://localhost:3000'));